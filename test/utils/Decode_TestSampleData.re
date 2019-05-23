let jsonNull: Js.Json.t = [%raw {| null |}];
let jsonBool: Js.Json.t = [%raw {| true |}];
let jsonString: Js.Json.t = [%raw {| "string" |}];
let jsonStringTrue: Js.Json.t = [%raw {| "true" |}];
let jsonFloat: Js.Json.t = [%raw {| 3.14 |}];
let jsonInt: Js.Json.t = [%raw {| 1 |}];
let jsonIntZero: Js.Json.t = [%raw {| 0 |}];
let jsonDateNumber: Js.Json.t = [%raw {| 1542433304450.0 |}];
let jsonDateString: Js.Json.t = [%raw {| "2018-11-17T05:40:35.869Z" |}];

let valBool = true;
let valString = "string";
let valFloat = 3.14;
let valInt = 1;
let valIntZero = 0;
let valDateNumber = Js.Date.fromFloat(1542433304450.0);
let valDateString = Js.Date.fromString("2018-11-17T05:40:35.869Z");
