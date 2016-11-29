# Vscale API
vscale-api is a C++ client library for accessing the [Vscale API](https://developers.vscale.io).

## Build

```bash
$ cmake CMakeLists.txt
$ make install
```

## Usage

```cpp
#include <vscale/vscale.h>
#include <iostream>

int main() {
  Json::Value response;
  try {
    vscale::Scalets("token").List(response);
    std::cout << response.toStyledStrin() << std::endl;
  } catch (BadRequest &e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
```

```bash
$ g++ -std=c++11 -Wall main.cpp -lvscale -lcppjson -o vscale-test
```
