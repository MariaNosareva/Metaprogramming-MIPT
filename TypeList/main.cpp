#include <iostream>

/* --------------------------------------------------- TYPELIST -----------------------------------------------------*/

class NullClass {};

template <typename T = NullClass, typename ... U>
struct TypeList {
  using head = T;
  using tail = TypeList<U ...>;
};

template <typename T>
struct TypeList<T> {
  using head = T;
  using tail = NullClass;
};

/* --------------------------------------------------- LENGTH -------------------------------------------------------*/

template <typename T>
struct Length {
  enum { value = 1 + Length<typename T::tail>::value };
};

template <>
struct Length<TypeList<>> {
  enum { value = 0 };
};

template <>
struct Length<NullClass> {
  enum { value = 0 };
};

/* --------------------------------------------------- TYPEAT -------------------------------------------------------*/

template <typename T, unsigned int index> struct TypeAt;

template <typename T>
struct TypeAt<T, 0> {
  using result = typename T::head;
};

template <typename T, unsigned int index>
struct TypeAt {
  using result = typename TypeAt<typename T::tail, index - 1>::result;
};

/* ---------------------------------------------------- ADD ---------------------------------------------------------*/

template <typename T, unsigned int index, typename E> struct AddType;

template <typename Head, typename Elem, typename ... Tail>
struct AddType<TypeList<Head, Tail ... >, 0, Elem> {
  using result = TypeList<Elem, Head, Tail ... >;
};

template <typename Head, typename Elem, unsigned int index, typename ... Tail>
struct AddType<TypeList<Head, Tail ...>, index, Elem> {
  using result =typename AddType<typename AddType<TypeList<Tail ...>, index - 1, Elem>::result, 0, Head>::result;
};


/* --------------------------------------------------- REMOVE -------------------------------------------------------*/

template <typename T, unsigned int index> struct RemoveType;

template <typename Head, typename ... Tail>
struct RemoveType<TypeList<Head, Tail ... >, 0> {
  using result = TypeList<Tail ... >;
};

template <typename Head, unsigned int index, typename ... Tail>
struct RemoveType<TypeList<Head, Tail ... >, index> {
  using result = typename AddType<typename RemoveType<TypeList<Tail ...>, index - 1>::result, 0, Head>::result;
};

/* --------------------------------------------------- CHANGE -------------------------------------------------------*/

template <typename T, unsigned int index, typename E> struct ChangeType;

template <typename Head, typename Elem, typename ... Tail>
struct ChangeType<TypeList<Head, Tail ...>, 0, Elem> {
  using result = typename AddType<TypeList<Tail ...>, 0, Elem>::result;
};

template <typename Head, typename Elem, unsigned int index, typename ... Tail>
struct ChangeType<TypeList<Head, Tail ...>, index, Elem> {
  using result = typename AddType<typename ChangeType<TypeList<Tail ...>, index - 1, Elem>::result, 0, Head>::result;
};


typedef TypeList<int, double, std::string> TL;

int main() {
  TypeAt<ChangeType<TL, 1, int>::result, 1>::result a = 3.5;
  return 0;
}