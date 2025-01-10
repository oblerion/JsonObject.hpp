Translate of [JsonObject.cs](https://github.com/oblerion/JsonObject.cs) for c++.<br>
With same functionnality and fix. 

## How use it

### your json file
```json
{
"x":5, "size": 0.5, "name":"jake", "point":{"x":12, "y":34}, 
"visible":true, "array":[0,2,4]
}
```
### your code
```cpp
#include "JsonObject.hpp"
#include <string>
#include <vector>

JsonObject jo("file.json");
if(jo.IsEmpty()==false)
{
  int x = jo.GetInt("x");            //-> 5
  float size = jo.GetFloat("size");  //-> 0,5
  std::string name = jo.GetString("name");//-> jake
  int point_x = jo.GetObject("point").GetInt("x"); // -> 12
  int point_y = jo.GetObject("point").GetInt("y"); // -> 34
  bool visible = jo.GetBool("visible"); //-> true
  std::vector<int> arr = jo.GetArray("array");
}
```
