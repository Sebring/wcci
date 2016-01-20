var express = require('express');
var Firebase = require("firebase");
var https = require('https');
var moment = require('moment');

var app = express();

var ref = new Firebase("https://[YOUR_INSTANCE_NAME].firebaseio.com/");
var visits_ref = ref.child("visits");
var visit_ref = null;

var AggregateType = {
  HOURS: 'hour',
  DAYS: 'day'
}

function push_event(state) {
  if (state == 1) {	//Locked
    visit_ref = visits_ref.push();
    //visit_ref.update({"startedAt": Firebase.ServerValue.TIMESTAMP});
    visit_ref.update({"startedAt": moment().valueOf()});
  }
  else {
    if (visit_ref != null) {
      //visit_ref.update({"endedAt": Firebase.ServerValue.TIMESTAMP});
      visit_ref.update({"endedAt": moment().valueOf()});
      visit_ref = null;
    }
  }
}


//****************************************************************
// Generic routes
//****************************************************************

app.get('/', function (req, res) {
  res.send('Welcome to WC Courtesy Indicator!');
});

app.get('/log_visit_state/:state', function (req, res) {
  res.json({"msg": "OK"});
  console.log(req.params.state);
  push_event(req.params.state);
});


//****************************************************************
// Routes for getting visits data
//****************************************************************

//Get all visits made today
app.get('/visits/today', function(req, res) {
  var startDate = moment().startOf('day');
  var endDate = moment().endOf('day');
  var aggregateBy = req.query.aggregateBy;
  
  getVisitsForInterval(startDate.valueOf(), endDate.valueOf(), res, aggregateBy);
});

//Get all visits made yesterday
app.get('/visits/yesterday', function(req, res) {
  var startDate = moment().subtract(1, 'days').startOf('day');
  var endDate = moment().subtract(1, 'days').endOf('day');
  var aggregateBy = req.query.aggregateBy;
  
  getVisitsForInterval(startDate.valueOf(), endDate.valueOf(), res, aggregateBy);
});

//Get latest visit
app.get('/visits/latest', function(req, res) {
  var aggregateBy = req.query.aggregateBy;
  getLatestVisit(res, aggregateBy);
});

//Get all visits made this week 
app.get('/visits/thisweek', function(req, res) {
  var startDate = moment().startOf('isoweek');
  var endDate = moment().endOf('isoweek');
  var aggregateBy = req.query.aggregateBy;
  
  getVisitsForInterval(startDate.valueOf(), endDate.valueOf(), res, aggregateBy);
});

//Get all visits made this month
app.get('/visits/thismonth', function(req, res) {
  var startDate = moment().startOf('month');
  var endDate = moment().endOf('month');
  var aggregateBy = req.query.aggregateBy;
  
  getVisitsForInterval(startDate.valueOf(), endDate.valueOf(), res, aggregateBy);
});

//Get all visits made last week 
app.get('/visits/lastweek', function(req, res) {
  var startDate = moment().subtract(1, 'weeks').startOf('isoweek');
  var endDate = moment().subtract(1, 'weeks').endOf('isoweek');
  var aggregateBy = req.query.aggregateBy;
  
  getVisitsForInterval(startDate.valueOf(), endDate.valueOf(), res, aggregateBy);
});

//Get all visits made last month
app.get('/visits/lastmonth', function(req, res) {
  var startDate = moment().subtract(1, 'months').startOf('month');
  var endDate = moment().subtract(1, 'months').endOf('month');
  var aggregateBy = req.query.aggregateBy;
  
  getVisitsForInterval(startDate.valueOf(), endDate.valueOf(), res, aggregateBy);
});

//Get all visits for a specific date
app.get("/visits/:date", function(req, res) {
  var startDate = moment(req.params.date);
  var endDate = moment(req.params.date).endOf('day');
  var aggregateBy = req.query.aggregateBy;
  
  getVisitsForInterval(startDate.valueOf(), endDate.valueOf(), res, aggregateBy);
});

//Get all visits for a specific start and end date
app.get("/visits/:start/:end", function(req, res) {
  var startDate = moment(req.params.start);
  var endDate = moment(req.params.end)
  var aggregateBy = req.query.aggregateBy;
  
  getVisitsForInterval(startDate.valueOf(), endDate.valueOf(), res, aggregateBy);
});

//****************************************************************
// Helpers
//****************************************************************

function getLatestVisit(res, aggregateBy) {
  ref.child("visits").orderByChild("startedAt").limitToLast(1).once("value", function(snapshot) {
    snapshot.forEach(function (data) {
      var duration = Math.floor((data.child("endedAt").val() - data.child("startedAt").val()) / 1000);
      
      res.json({
        startedAt: moment(data.child("startedAt").val()).format(),
        endedAt: moment(data.child("endedAt").val()).format(),
        duration: duration
      });
    });
  });
}

function getVisitsForInterval(startDate, endDate, res, aggregateBy) {
  var visits = [];
  var visitsDuration = [];
  var averageDuration, longestVisit, shortestVisit;
  
  console.log(moment(startDate).format(), moment(endDate).format());
  
  //Get visits
  ref.child("visits").orderByChild("startedAt").startAt(startDate).endAt(endDate).once("value", function(snapshot) {
    snapshot.forEach(function (data) {
      if (data.child("endedAt").exists()) {
        var duration = Math.floor((data.child("endedAt").val() - data.child("startedAt").val()) / 1000);
        visits.push(
          {
            startedAt: moment(data.child("startedAt").val()).format(),
            endedAt: moment(data.child("endedAt").val()).format(),
            duration: duration
          }
        );
        visitsDuration.push(duration);
      }
    });
    
    //Get calculated data
    averageDuration = getAverage(visitsDuration);
    longestVisit = Math.max.apply(Math, visitsDuration);
    shortestVisit = Math.min.apply(Math, visitsDuration);
    
    var json = {
      start: moment(startDate).format(), 
      end: moment(endDate).format(),
      averageDuration: averageDuration,
      longestVisit: longestVisit,
      shortestVisit: shortestVisit
    }
    
    if (aggregateBy === AggregateType.HOURS || aggregateBy === AggregateType.DAYS) {
      //Send aggregated visits response
      var aggregatedVisits = getAggregatedVisits(visits, aggregateBy);
      json.visits = aggregatedVisits;
    }
    else { 
      //Send visits response
      json.visits = visits;
    }
    
    res.json(json);
  });
}

function getAggregatedVisits(visits, aggregateBy) {
  var agg = [];
  var duration = 0;
  var count = 0;
  
  console.log("Aggregate by", aggregateBy);
  
  //Days
  if (aggregateBy === AggregateType.DAYS) {
    var currentDay = moment(visits[0].startedAt);
  
    visits.forEach(function(visit) {
      if (moment(visit.startedAt).isSame(moment(currentDay).startOf('day'), 'day')) {
        duration += visit.duration;
        count++;
      }
      else {
        agg.push({day: moment(currentDay).startOf('day').format('YYYY-MM-DD'), count: count, duration: duration});
        duration = visit.duration;
        count = 1;
        currentDay = visit.startedAt;
      }
    });
    agg.push({day: moment(currentDay).startOf('day').format('YYYY-MM-DD'), count: count, duration: duration});
  }
  
  //Hours
  else if (aggregateBy === AggregateType.HOURS) {
    var currentHour = moment(visits[0].startedAt).hour();
    var hours = [];
  
    visits.forEach(function(visit) {
      if (moment(visit.startedAt).hour() === currentHour) {
        duration += visit.duration;
        count++;
      }
      else {
        hours.push({hour: currentHour, count: count, duration: duration});
        agg.push({day: moment(visit.startedAt).format('YYYY-MM-DD'), hour: currentHour, count: count, duration: duration});
        duration = visit.duration;
        count = 1;
        currentHour = moment(visit.startedAt).hour();
      }
    });
    agg.push({day: moment(visits[visits.length-1].startedAt).format('YYYY-MM-DD'), hour: currentHour, count: count, duration: duration});
  }
  return agg;
}

function getAggregatedHoursPerDay() {
  
}

function getAverage(collection) {
  var total = 0;
  var result = 0;
  
  if (collection.length == 0) return 0;
  
  total = collection.reduce(function(a, b) {
    return a + b;
  });
  
  result = Math.round(total / collection.length);
  
  return result;
}

//****************************************************************
// Server
//****************************************************************
//Start your engines!
var server = app.listen(3000, function () {
  var host = server.address().address;
  var port = server.address().port;

  console.log('WCCI is listening at http://%s:%s', host, port);
});