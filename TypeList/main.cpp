#include <iostream>
#include <typeinfo>

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

/* --------------------------------------------------- PRINT --------------------------------------------------------*/

template <typename TL>
void printTL() {
  std::cout << typeid(typename TL::head).name() << " ";
  printTL<typename TL::tail>();
};

template <>
void printTL<NullClass>() {
  return;
}

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

/* -------------------------------------------------- CHANGE --------------------------------------------------------*/

template <typename T, unsigned int index, typename E> struct ChangeType;

template <typename Head, typename Elem, typename ... Tail>
struct ChangeType<TypeList<Head, Tail ...>, 0, Elem> {
  using result = typename AddType<TypeList<Tail ...>, 0, Elem>::result;
};

template <typename Head, typename Elem, unsigned int index, typename ... Tail>
struct ChangeType<TypeList<Head, Tail ...>, index, Elem> {
  using result = typename AddType<typename ChangeType<TypeList<Tail ...>, index - 1, Elem>::result, 0, Head>::result;
};

/* ------------------------------------------- GET FIRST N ELEMENTS  ------------------------------------------------*/

template <typename TL, unsigned int HowMany> struct CutFirstN;

template <typename TL, unsigned int HowMany>
struct CutFirstN {
  using result = typename AddType<typename CutFirstN<TL, HowMany - 1>::result,
                                  HowMany - 1,
                                  typename TypeAt<TL, HowMany - 1>::result>::result;
};

template <typename TL>
struct CutFirstN<TL, 0> {
  using result = TypeList<>;
};

/* --------------------------------------- GENERATING SCATTER HIERARHY ----------------------------------------------*/

template <class T>
struct Unit {
  T value;
};

template <typename TL, template <typename> typename U>  struct GSH;

template <typename Head, template <typename> typename U, typename ... Tail>
struct GSH<TypeList<Head, Tail ...>, U>: public U<Head>,
            public GSH<TypeList<Tail ...>, U> {
  using TL = TypeList<Head, Tail ...>;
  using RightTree = U<Head>;
  using LeftTree = GSH<TypeList<Tail ...>, U>;
};

template <template <typename> typename U>
struct GSH<TypeList<>, U> {  };

/* --------------------------------------- GENERATING LINEAR HIERARHY -----------------------------------------------*/

template <class T, class Base>
struct LinearUnit: Base {
  T value;
};

template <typename TL, template <class, class> typename U, class Root = NullClass>  struct GLH;

template <typename Head, template <class, class> typename U, class Root, typename ... Tail>
struct GLH<TypeList<Head, Tail ...>, U, Root>: public U<Head, GLH<TypeList<Tail ...>, U, Root>> {};

template <typename Tail, template <class, class> typename U, class Root>
struct GLH<TypeList<Tail>, U, Root>: public LinearUnit<Tail, Root> {};

/* --------------------------------------- GENERATING FIBONACCI NUMBERS ---------------------------------------------*/

template <int T>
struct Fibonacci {
  enum { value = Fibonacci<T - 1>::value + Fibonacci<T - 2>::value };
};

template <>
struct Fibonacci<1> {
  enum { value = 1 };
};

template <>
struct Fibonacci<2> {
  enum { value = 1 };
};

/*-------------------------------------------------------------------------------------------------------------------*/

class First {
 public:
  std::string fisrtString = "hello";
  std::string getFirstString() const {
    return fisrtString;
  }
};

typedef TypeList<First, std::string, First> TL;
typedef GSH<TL, Unit> SH;
typedef GLH<TL, LinearUnit> LH;
// typedef CutFirstN<TL, 2>::result CuttedTL;

int main() {
  return 0;
}