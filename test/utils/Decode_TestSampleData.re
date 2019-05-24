// Simple JSON values
let jsonNull: Js.Json.t = [%raw {| null |}];
let jsonBool: Js.Json.t = [%raw {| true |}];
let jsonString: Js.Json.t = [%raw {| "string" |}];
let jsonStringTrue: Js.Json.t = [%raw {| "true" |}];
let jsonFloat: Js.Json.t = [%raw {| 3.14 |}];
let jsonInt: Js.Json.t = [%raw {| 1 |}];
let jsonIntZero: Js.Json.t = [%raw {| 0 |}];
let jsonDateNumber: Js.Json.t = [%raw {| 1542433304450.0 |}];
let jsonDateString: Js.Json.t = [%raw {| "2018-11-17T05:40:35.869Z" |}];

// Simple typed values
let valBool = true;
let valString = "string";
let valFloat = 3.14;
let valInt = 1;
let valIntZero = 0;
let valDateNumber = Js.Date.fromFloat(1542433304450.0);
let valDateString = Js.Date.fromString("2018-11-17T05:40:35.869Z");

// Nested JSON values
let jsonArrayEmpty: Js.Json.t = [%raw {| [] |}];
let jsonArrayString: Js.Json.t = [%raw {| ["A", "B", "C"] |}];

// Nested typed values
let valArrayEmpty = [||];
let valArrayString = [|"A", "B", "C"|];
let valListEmpty = [];
let valListString = ["A", "B", "C"];

// JSON variant values
let jsonStringBlue: Js.Json.t = [%raw {| "blue" |}];
let jsonStringYellow: Js.Json.t = [%raw {| "yellow" |}];
let jsonIntFive: Js.Json.t = [%raw {| 5 |}];

// typed variants and converters
[@bs.deriving jsConverter]
type color = [ | `blue | `red | `green];

[@bs.deriving jsConverter]
type numbers =
  | Zero
  | One
  | Two;

// JSON object values
let jsonDictEmpty: Js.Json.t = [%raw {| {} |}];
let jsonDictFloat: Js.Json.t = [%raw
  {|
  {
    "key1": 3.14,
    "key2": 2.22,
    "key3": 100.0
  }
|}
];

let jsonJobCeo: Js.Json.t = [%raw
  {|
  {
    "title": "CEO",
    "companyName": "My Company",
    "startDate": "2016-04-01T00:00:00.0Z",
    "manager": null
  }
|}
];

let jsonPersonBill: Js.Json.t = [%raw
  {|
  {
    "name": "Bill",
    "age": 27,
    "job": {
      "title": "Designer",
      "companyName": "My Company",
      "startDate": "2018-11-17T05:40:35.869Z",
      "manager": {
        "name": "Jane",
        "age": 38,
        "job": {
          "title": "CEO",
          "companyName": "My Company",
          "startDate": "2016-04-01T00:00:00.0Z",
        }
      }
    }
  }
|}
];

// Typed dicts and records
let valDictEmpty: Js.Dict.t(string) = Js.Dict.empty();
let valDictFloat =
  Js.Dict.fromArray([|("key1", 3.14), ("key2", 2.22), ("key3", 100.0)|]);

type job = {
  title: string,
  companyName: string,
  startDate: Js.Date.t,
  manager: option(employee),
}
and employee = {
  name: string,
  age: int,
  job,
};

let makeJob = (title, companyName, startDate, manager) => {
  title,
  companyName,
  startDate,
  manager,
};

let makeEmployee = (name, age, job) => {name, age, job};

let jobCeo =
  makeJob(
    "CEO",
    "My Company",
    Js.Date.fromString("2016-04-01T00:00:00.0Z"),
    None,
  );

let employeeJane = makeEmployee("Jane", 38, jobCeo);

let jobDesigner =
  makeJob("Designer", "My Company", valDateString, Some(employeeJane));

let employeeBill = makeEmployee("Bill", 27, jobDesigner);

// Typed union for oneOf
type union =
  | S(string)
  | N(option(float))
  | B(bool);

let unionS = v => S(v);
let unionN = v => N(v);
let unionB = v => B(v);
