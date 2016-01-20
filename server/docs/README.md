# WCCI API

## Description
WCCI API is a node application for getting visit data and exposing it in a number of methods. The data is located in a Firebase instance, but could just as well be hosted elsewhere. 

## Data structure
All visits are saved under a "visits" node, located directly under the Firebase instance root.

A response from either of the methods listed below looks like this (excluding the comments):  

    {
        "start":"2015-11-26T00:00:00+01:00",    //The start of the requested timeframe/interval
        "end":"2015-11-26T23:59:59+01:00",      //The end of the requested timeframe/interval
        "averageDuration":10,                   //Average duration, in seconds
        "longestVisit":24,                      //Longest visit, in seconds
        "shortestVisit":2,                      //Shortest visit, in seconds
        "visits":[                              //An array of the individual visits
            {"startedAt":"2015-11-26T11:42:07+01:00","endedAt":"2015-11-26T11:42:32+01:00","duration":24},
            {"startedAt":"2015-11-26T16:14:59+01:00","endedAt":"2015-11-26T16:15:01+01:00","duration":2},
            {"startedAt":"2015-11-26T16:15:30+01:00","endedAt":"2015-11-26T16:15:35+01:00","duration":4}
        ]
    }

## Methods
### GET
`/log_visit_state/:state[0 | 1]`  
Log a new visit state. Lock = 1, unlock = 0.

`/visits/[today | yesterday | latest | thisweek | thismonth | lastweek | lastmonth]`  
Get all visits made during the specified timeframe.

`/visits/:date`  
Get all visits made during the specified date.

`/visits/:start/:end`  
Get all visits made during the specified interval, expressed in `yyyy-mm-dd`

### POST
n/a

## Aggregation
All methods can have an additional query parameter for aggregation, by hour or day:  
`?aggregateBy=[hour | day]`

**Example:**  
`/visits/thisweek/?aggregateBy=hour`  
Gets all visits made during the current week, aggregated by hour.


That's all folks!