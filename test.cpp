// #include <map>
// #include <iostream>

// class B {
// public:
//   B(int val) {i = val;}
//   B(const B &cpy) {*this = cpy;}
//   B &operator=(const B &other) {
//     if (this != &other) {
//       i = other.i;
//     }
//     return *this;
//   }
//   ~B() {}

//   void kill_me(std::map<int, B>& map) {
//     map.erase(i);
//   }

// private:
//   int i;
// };


// class A {
// public:
//   A() {}
//   A(const A &cpy) { *this = cpy; }
//   A &operator=(const A &other) {
//     if (this != &other){
//       map = other.map;
//     }
//     return *this;
//   }
//   ~A() {}

//   std::map<int, B> map;
// };

// int main(void) {
//   A a;
//   size_t idx = 0;
//   while (true) {  
//     a.map.insert(std::pair<int, B>(idx, B(idx)));
//     a.map.at(idx).kill_me(a.map);
//     idx++;
//   }

//   return 0;
// }
