#include "formulis/formulis.hpp"

#include <catch2/catch_test_macros.hpp>

auto my_func(const int& x) -> int
{
  return 3 * x + 1;
}
REGISTER_UNARY_PROC(my_func, int)

auto is_even(const int& x) -> bool
{
  return x % 2 == 0;
}
REGISTER_UNARY_PROC_BOOL(is_even, int)

TEST_CASE("simple terms", "[simple]")
{
  SECTION("change from 2 to 3")
  {
    term x(2);
    REQUIRE(x == 2);
    x.set(3);
    REQUIRE(x == 3);
  }
  SECTION("increment test")
  {
    term x(2);
    REQUIRE(x == 2);
    ++x;
    REQUIRE(x == 3);
    int y = x++;
    REQUIRE(y == 3);
    REQUIRE(x == 4);
  }
  SECTION("decrement test")
  {
    term x(4);
    REQUIRE(x == 4);
    --x;
    REQUIRE(x == 3);
    int y = x--;
    REQUIRE(y == 3);
    REQUIRE(x == 2);
  }
  SECTION("+= test")
  {
    term x(2);
    REQUIRE(x == 2);
    x += 2;
    REQUIRE(x == 4);
  }
  SECTION("-= test")
  {
    term x(4);
    REQUIRE(x == 4);
    x -= 2;
    REQUIRE(x == 2);
  }
}

TEST_CASE("2 term formulas", "[simple]")
{
  SECTION("basic formula [+]")
  {
    term x(2);
    term y(2);
    auto z = x + y;
    REQUIRE(z == 4);
    x.set(3);
    REQUIRE(z == 5);
    y.set(3);
    REQUIRE(z == 6);
  }

  SECTION("basic formula [+] and const")
  {
    term x(2);
    auto z = x + 1;
    REQUIRE(z == 3);
    x.set(3);
    REQUIRE(z == 4);
    auto w = 1 + x;
    REQUIRE(w == 4);
    x.set(4);
    REQUIRE(w == 5);
  }

  SECTION("basic formula [*]")
  {
    term x(2);
    term y(2);
    auto z = x * y;
    REQUIRE(z == 4);
    x.set(3);
    REQUIRE(z == 6);
    y.set(3);
    REQUIRE(z == 9);
  }

  SECTION("basic formula [/]")
  {
    term x(2);
    term y(2);
    auto z = x / y;
    REQUIRE(z == 1);
    x.set(3);
    REQUIRE(z == 1);
    y.set(4);
    REQUIRE(z == 0);
  }

  SECTION("basic formula [%]")
  {
    term x(2);
    term y(2);
    auto z = x % y;
    REQUIRE(z == 0);
    x.set(3);
    REQUIRE(z == 1);
    y.set(4);
    REQUIRE(z == 3);
  }
}

TEST_CASE("3 term formulas", "[simple]")
{
  SECTION("test formula creation and updating")
  {
    term x(2);
    term y(2);
    term z(2);
    auto w = x + y + z;
    REQUIRE(w == 6);
    x.set(3);
    REQUIRE(w == 7);
    y.set(3);
    REQUIRE(w == 8);
    z.set(3);
    REQUIRE(w == 9);
  }
  SECTION("test formula creation and updating (different parenthesis)")
  {
    term x(2);
    term y(2);
    term z(2);
    auto w = x + (y + z);
    REQUIRE(w == 6);
    x.set(3);
    REQUIRE(w == 7);
    y.set(3);
    REQUIRE(w == 8);
    z.set(3);
    REQUIRE(w == 9);
  }
}

TEST_CASE("4 term formulas", "[simple]")
{
  SECTION("test formula creation and updating")
  {
    term x(2);
    term y(2);
    term z(2);
    term w(2);
    auto o = x + y + z + w;
    REQUIRE(o == 8);
    x.set(3);
    REQUIRE(o == 9);
    y.set(3);
    REQUIRE(o == 10);
    z.set(3);
    REQUIRE(o == 11);
    w.set(3);
    REQUIRE(o == 12);
  }
  SECTION("test formula creation and updating (different parenthesis)")
  {
    term x(2);
    term y(2);
    term z(2);
    term w(2);
    auto o = (x + y) + (z + w);
    REQUIRE(o == 8);
    x.set(3);
    REQUIRE(o == 9);
    y.set(3);
    REQUIRE(o == 10);
    z.set(3);
    REQUIRE(o == 11);
    w.set(3);
    REQUIRE(o == 12);
  }
}

TEST_CASE("constants in formulas", "[const]")
{
  SECTION("constant on the right updates")
  {
    term x(2);
    auto z = x + 1;
    REQUIRE(z == 3);
    x.set(5);
    REQUIRE(z == 6);
  }

  SECTION("constant on the left updates")
  {
    term x(2);
    auto z = 10 - x;
    REQUIRE(z == 8);
    x.set(4);
    REQUIRE(z == 6);
  }

  SECTION("multiple constants in one expression")
  {
    term x(1);
    auto z = x + 1 + 2 + 3;
    REQUIRE(z == 7);
    x.set(4);
    REQUIRE(z == 10);
  }

  SECTION("several formulas sharing one term with constants")
  {
    term x(2);
    auto a = x + 1;
    auto b = x * 5;
    auto c = x - 3;
    REQUIRE(a == 3);
    REQUIRE(b == 10);
    REQUIRE(c == -1);
    x.set(7);
    REQUIRE(a == 8);
    REQUIRE(b == 35);
    REQUIRE(c == 4);
  }

  SECTION("constants nested with parenthesis")
  {
    term a(2);
    term b(3);
    auto z = (a + 1) * (b + 2);
    REQUIRE(z == 15);
    a.set(1);
    REQUIRE(z == 10);
    b.set(4);
    REQUIRE(z == 12);
  }

  SECTION("term copies are independent")
  {
    term x(2);
    term y = x;
    REQUIRE(y == 2);
    auto z = x + 1;
    x.set(10);
    REQUIRE(z == 11);
    REQUIRE(y == 2);
    ++x;
    REQUIRE(z == 12);
    REQUIRE(y == 2);
  }
}

TEST_CASE("boolean operators", "[bool]")
{
  SECTION("term && term")
  {
    term x(1);
    term y(0);
    auto b = x && y;
    REQUIRE(b == 0);
    y.set(2);
    REQUIRE(b == 1);
    x.set(0);
    REQUIRE(b == 0);
  }

  SECTION("term || term")
  {
    term x(0);
    term y(0);
    auto b = x || y;
    REQUIRE(b == 0);
    x.set(3);
    REQUIRE(b == 1);
  }

  SECTION("constant && term")
  {
    term x(0);
    auto b = 1 && x;
    REQUIRE(b == 0);
    x.set(4);
    REQUIRE(b == 1);
  }

  SECTION("term || constant")
  {
    term x(4);
    auto b = x || 0;
    REQUIRE(b == 1);
    x.set(0);
    REQUIRE(b == 0);
  }

  SECTION("not term")
  {
    term x(1);
    auto b = !x;
    REQUIRE(b == 0);
    x.set(0);
    REQUIRE(b == 1);
  }

  SECTION("not formula")
  {
    term x(2);
    auto f = x - 2;
    auto b = !f;
    REQUIRE(b == 1);
    x.set(5);
    REQUIRE(b == 0);
  }

  SECTION("mixed formulas and constants")
  {
    term x(1);
    term y(1);
    auto b = (x && y) || 0;
    REQUIRE(b == 1);
    x.set(0);
    REQUIRE(b == 0);
    y.set(5);
    REQUIRE(b == 0);
    x.set(2);
    REQUIRE(b == 1);
  }

  SECTION("bool terms")
  {
    term p(true);
    term q(false);
    auto b = p && q;
    REQUIRE(b == false);
    q.set(true);
    REQUIRE(b == true);
  }
}

TEST_CASE("custom boolean operator", "[bool]")
{
  SECTION("test creation of custom boolean unary op")
  {
    term x(4);
    auto z = is_even(x);
    REQUIRE(z == 1);
    x.set(5);
    REQUIRE(z == 0);
  }
}

TEST_CASE("unary operators", "[unary]")
{
  SECTION("unary minus on term is reactive")
  {
    term x(5);
    auto z = -x;
    REQUIRE(z == -5);
    x.set(-7);
    REQUIRE(z == 7);
  }

  SECTION("unary plus on term is reactive")
  {
    term x(5);
    auto z = +x;
    REQUIRE(z == 5);
    x.set(-3);
    REQUIRE(z == -3);
  }

  SECTION("unary minus on formula")
  {
    term x(5);
    auto f = x * 2;
    auto z = -f;
    REQUIRE(z == -10);
    x.set(10);
    REQUIRE(z == -20);
  }

  SECTION("unary minus on expression with constant")
  {
    term x(2);
    auto z = -(x + 3);
    REQUIRE(z == -5);
    x.set(4);
    REQUIRE(z == -7);
  }

  SECTION("bitwise not on term")
  {
    term x(0);
    auto z = ~x;
    REQUIRE(z == -1);
    x.set(5);
    REQUIRE(z == -6);
  }
}

TEST_CASE("set from formula and term", "[simple]")
{
  SECTION("set from formula takes its current value")
  {
    term x(1);
    term y(2);
    auto f = x + 41;
    y.set(f);
    REQUIRE(y == 42);
    x.set(0);
    REQUIRE(f == 41);
    REQUIRE(y == 42);
  }

  SECTION("set from term takes its current value")
  {
    term a(5);
    term b(1);
    b.set(a);
    REQUIRE(b == 5);
    a.set(9);
    REQUIRE(b == 5);
  }
}

TEST_CASE("on_change", "[on_change]")
{
  SECTION("fires for auto-bound formula")
  {
    term x(3);
    term y(5);
    auto z = x + y;
    int old_v = -1;
    int new_v = -1;
    z.on_change(
        [&old_v, &new_v](int o, int n)
        {
          old_v = o;
          new_v = n;
        });
    x.set(4);
    REQUIRE(old_v == 8);
    REQUIRE(new_v == 9);
    y.set(6);
    REQUIRE(old_v == 9);
    REQUIRE(new_v == 10);
  }

  SECTION("fires when registered on a copy of a copy")
  {
    term x(1);
    auto z = x + 1;
    auto z2 = z;
    int calls = 0;
    z2.on_change([&calls](int, int) { ++calls; });
    x.set(5);
    REQUIRE(calls == 1);
  }

  SECTION("multiple listeners on the same formula")
  {
    term x(0);
    auto z = x + 10;
    int calls = 0;
    int sum = 0;
    z.on_change([&calls](int, int) { ++calls; });
    z.on_change([&sum](int, int n) { sum += n; });
    x.set(1);
    REQUIRE(calls == 1);
    REQUIRE(sum == 11);
  }

  SECTION("operator applied to auto-bound formula stays reactive")
  {
    term x(2);
    auto z = x + 1;
    auto w = z * 3;
    REQUIRE(w == 9);
    x.set(3);
    REQUIRE(w == 12);
  }

  SECTION("listener on formula built from auto-bound formula")
  {
    term x(2);
    auto z = x + 1;
    auto w = z * 3;
    int calls = 0;
    w.on_change([&calls](int, int) { ++calls; });
    x.set(3);
    REQUIRE(w == 12);
    REQUIRE(calls == 1);
  }
}

TEST_CASE("custom operator", "[simple]")
{
  SECTION("test creation of custom unary op")
  {
    term x(3);
    auto z = my_func(x);
    REQUIRE(z == 10);
    x.set(4);
    REQUIRE(z == 13);
  }
}
