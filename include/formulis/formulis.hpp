#include <algorithm>
#include <functional>
#include <variant>
#include <vector>

// See below for docs on this.
template<class... Ts>
struct overloaded : Ts...
{
  using Ts::operator()...;
};
// For C++17 compatibility.
/**
 * Helper to combine multiple lambdas, used for "pattern matching."
 */
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * operator overloading when the lhs is a numeric. As in `auto z = 1 + x`.
 */
#define REGISTER_OVERLOAD_VAL_LEFT(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->formula<T>& \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> T \
    { return lhss name rhss; }; \
    const_<T>* new_const = new const_<T>(lhs); \
    const bin_expr<T> new_expr = {new_const, node_of(rhs), comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    new_const->m_parents.push_back(form); \
    node_of(rhs)->m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * operator overloads when the lhs is a numeric/bool. As in `auto z = true ||
 * x`. Note: The result is a formula over the operand type T storing a boolean
 * (i.e. `true`/`false` for bool operands, `1`/`0` otherwise).
 */
#define REGISTER_OVERLOAD_VAL_LEFT_BOOL(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->formula<T>& \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> bool \
    { return lhss name rhss; }; \
    const_<T>* new_const = new const_<T>(lhs); \
    const bin_expr<T> new_expr = {new_const, &rhs, comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    new_const->m_parents.push_back(form); \
    node_of(rhs)->m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * operator overloading when the rhs is a numeric. As in `auto z = x + 1`.
 */
#define REGISTER_OVERLOAD_VAL_RIGHT(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->formula<T>& \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> T \
    { return lhss name rhss; }; \
    const_<T>* new_const = new const_<T>(rhs); \
    const bin_expr<T> new_expr = {node_of(lhs), new_const, comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    node_of(lhs)->m_parents.push_back(form); \
    new_const->m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to generate operator overloads. In particular, this creates
 * operator overloads when the rhs is a numeric/bool. As in `auto z = x ||
 * true`. Note: The result is a formula over the operand type T storing a
 * boolean (i.e. `true`/`false` for bool operands, `1`/`0` otherwise).
 */
#define REGISTER_OVERLOAD_VAL_RIGHT_BOOL(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->formula<T>& \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> bool \
    { return lhss name rhss; }; \
    const_<T>* new_const = new const_<T>(rhs); \
    const bin_expr<T> new_expr = {node_of(lhs), new_const, comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    node_of(lhs)->m_parents.push_back(form); \
    new_const->m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to generate operator overloads. In particular, this creates a
 * particular arithmetic binary operator overload given the operator and two
 * types.
 */
#define REGISTER_BIN_OVERLOAD(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->formula<T>& \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> T \
    { return lhss name rhss; }; \
    const bin_expr<T> new_expr = {node_of(lhs), node_of(rhs), comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    node_of(lhs)->m_parents.push_back(form); \
    node_of(rhs)->m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to generate operator overloads. In particular, this creates a
 * particular boolean binary operator overload given the operator and two types.
 * Note: The result is a formula over the operand type T storing a boolean
 * (i.e. `true`/`false` for bool operands, `1`/`0` otherwise).
 */
#define REGISTER_BIN_OVERLOAD_BOOL(name, t1, t2) \
  template<typename T> \
  auto operator name(t1& lhs, t2& rhs)->formula<T>& \
  { \
    auto comb = [](const auto& lhss, const auto& rhss) -> bool \
    { return lhss name rhss; }; \
    const bin_expr<T> new_expr = {node_of(lhs), node_of(rhs), comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    node_of(lhs)->m_parents.push_back(form); \
    node_of(rhs)->m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to gnerate operator overloads. In particular, generates every
 * operator overload for a particular binary arithmetic operator.
 */
#define REGISTER_BIN_OP(op) \
  REGISTER_BIN_OVERLOAD(op, term<T>, formula<T>) \
  REGISTER_BIN_OVERLOAD(op, formula<T>, term<T>) \
  REGISTER_BIN_OVERLOAD(op, formula<T>, formula<T>) \
  REGISTER_OVERLOAD_VAL_LEFT(op, const T, formula<T>) \
  REGISTER_OVERLOAD_VAL_RIGHT(op, formula<T>, const T) \
  REGISTER_OVERLOAD_VAL_LEFT(op, const T, term<T>) \
  REGISTER_OVERLOAD_VAL_RIGHT(op, term<T>, const T) \
  REGISTER_BIN_OVERLOAD(op, term<T>, term<T>)

/**
 * Helper macro to gnerate operator overloads. In particular, generates every
 * operator overload for a particular binary boolean operator.
 */
#define REGISTER_BIN_OP_BOOL(op) \
  REGISTER_BIN_OVERLOAD_BOOL(op, term<T>, formula<T>) \
  REGISTER_BIN_OVERLOAD_BOOL(op, formula<T>, term<T>) \
  REGISTER_BIN_OVERLOAD_BOOL(op, formula<T>, formula<T>) \
  REGISTER_OVERLOAD_VAL_LEFT_BOOL(op, const T, formula<T>) \
  REGISTER_OVERLOAD_VAL_RIGHT_BOOL(op, formula<T>, const T) \
  REGISTER_OVERLOAD_VAL_LEFT_BOOL(op, const T, term<T>) \
  REGISTER_OVERLOAD_VAL_RIGHT_BOOL(op, term<T>, const T) \
  REGISTER_BIN_OVERLOAD_BOOL(op, term<T>, term<T>)

/**
 * Helper macro to generate operator overloads. In particular, this creates a
 * particular arithmetic unary operator overload given the operator and a type.
 */
#define REGISTER_UNARY_OVERLOAD(name, t1) \
  template<typename T> \
  auto operator name(t1& rhs)->formula<T>& \
  { \
    auto comb = [](const auto& rhss) -> T { return name rhss; }; \
    const unary_expr<T> new_expr = {node_of(rhs), comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    node_of(rhs)->m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to generate operator overloads. In particular, this creates a
 * particular boolean unary operator overload given the operator and a type.
 * Note: The result is a formula over the operand type T storing a boolean
 * (i.e. `true`/`false` for bool operands, `1`/`0` otherwise).
 */
#define REGISTER_UNARY_OVERLOAD_BOOL(name, t1) \
  template<typename T> \
  auto operator name(t1& rhs)->formula<T>& \
  { \
    auto comb = [](const auto& rhss) -> bool { return name rhss; }; \
    const unary_expr<T> new_expr = {node_of(rhs), comb}; \
    formula<T>* form = new formula<T>(new_expr); \
    node_of(rhs)->m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to gnerate operator overloads. In particular, generates every
 * operator overload for a particular unary arithmetic operator.
 */
#define REGISTER_UNARY_OP(op) \
  REGISTER_UNARY_OVERLOAD(op, term<T>) \
  REGISTER_UNARY_OVERLOAD(op, formula<T>)

/**
 * Helper macro to gnerate operator overloads. In particular, generates every
 * operator overload for a particular unary boolean operator.
 */
#define REGISTER_UNARY_OP_BOOL(op) \
  REGISTER_UNARY_OVERLOAD_BOOL(op, term<T>) \
  REGISTER_UNARY_OVERLOAD_BOOL(op, formula<T>)

#define REGISTER_INPLACE(name) \
  auto operator name(const term<T>& other)->term<T>& \
  { \
    T val = this->unwrap(); \
    val name other.unwrap(); \
    this->set(val); \
    return *this; \
  } \
  auto operator name(const formula<T>& other)->term<T>& \
  { \
    T val = this->unwrap(); \
    val name other.eval(); \
    this->set(val); \
    return *this; \
  } \
  auto operator name(const T& other)->term<T>& \
  { \
    T val = this->unwrap(); \
    val name other; \
    this->set(val); \
    return *this; \
  }

/**
 * Helper macro to generate operator overloads. In particular, this creates a
 * particular unary operator overload given the operator and a type.
 */
#define REGISTER_UNARY_PROC_OVERLOAD(name, t1, t2) \
  auto name(t1& rhs) -> formula<t2>& \
  { \
    auto comb = [](const auto& rhss) -> t2 { return name(rhss); }; \
    const unary_expr<t2> new_expr = {node_of(rhs), comb}; \
    formula<t2>* form = new formula<t2>(new_expr); \
    node_of(rhs)->m_parents.push_back(form); \
    return *form; \
  }

/**
 * Helper macro to generate operator overloads. In particular, this creates a
 * particular boolean unary operator overload given the operator and a type.
 * Note: The result is a formula over the operand type t2 storing a boolean
 * (i.e. `true`/`false` for bool operands, `1`/`0` otherwise).
 */
#define REGISTER_UNARY_PROC_OVERLOAD_BOOL(name, t1, t2) \
  auto name(t1& rhs) -> formula<t2>& \
  { \
    auto comb = [](const auto& rhss) -> bool { return name(rhss); }; \
    const unary_expr<t2> new_expr = {node_of(rhs), comb}; \
    formula<t2>* form = new formula<t2>(new_expr); \
    node_of(rhs)->m_parents.push_back(form); \
    return *form; \
  }

/**
 * Allow for registering functions (of one parameter).
 */
#define REGISTER_UNARY_PROC(name, t1) \
  REGISTER_UNARY_PROC_OVERLOAD(name, term<t1>, t1) \
  REGISTER_UNARY_PROC_OVERLOAD(name, formula<t1>, t1)

/**
 * Allow for registering functions (of one parameter) that return a boolean.
 * The resulting formula stores a boolean (i.e. `true`/`false` for bool
 * operands, `1`/`0` otherwise).
 */
#define REGISTER_UNARY_PROC_BOOL(name, t1) \
  REGISTER_UNARY_PROC_OVERLOAD_BOOL(name, term<t1>, t1) \
  REGISTER_UNARY_PROC_OVERLOAD_BOOL(name, formula<t1>, t1)

template<typename T>
struct unary_expr;

template<typename T>
struct bin_expr;

template<typename T>
class formula;

template<typename T>
class const_;

/**
 * `term` objects and `formula` objects are two of the fundamental types.
 * `term` objects store a value that can be changed over time. Importantly,
 * when they change, parent formula are also changed *automatically*. In this
 * context, a "parent" formula simply uses the term. So if `formula z = x + y;`
 * then `x` and `y` are terms that may change and `z` stores the result of
 * adding `x` and `y`.
 */
template<typename T>
class term
{
  std::vector<std::function<void(T, T)>> m_on_change;
  T m_value;

  auto change_detected(T old_value)
  {
    // Things changed. Send message to parents.
    for (const auto& func : m_on_change) {
      func(old_value, m_value);
    }
    // Propagate changes to parents.
    for (auto& parent : m_parents) {
      parent->set_needs_update();
      parent->update();
    }
  }

public:
  /** Do not touch directly! Needs to be public for user to use
   * REGISTER_*_OP(...) */
  std::vector<formula<T>*> m_parents;

  explicit term(T value)
      : m_parents({})
      , m_on_change({})
      , m_value(value)
  {
  }

  /**
   * Copying a term copies only its value: the copy starts out with no parent
   * formulae nor listeners of its own. Sharing parent links would be unsafe,
   * since destroying a term also destroys the parent formulae that use it.
   */
  term(const term<T>& other)
      : m_parents({})
      , m_on_change({})
      , m_value(other.m_value)
  {
  }

  term<T>& operator=(const term<T>& other)
  {
    this->set(other.m_value);
    return *this;
  }

  template<typename U>
  auto remove_parent(formula<U>* parent) -> void
  {
    m_parents.erase(std::remove(m_parents.begin(), m_parents.end(), parent));
  }

  ~term()
  {
    // Destroy the parent formulae. A copy of the parent list is used since
    // destroying a parent removes it from this list.
    auto parents = m_parents;
    for (auto* parent : parents) {
      delete parent;
    }
  }

  /**
   * Get the current value of the term. (For `formula` use `eval`).
   */
  auto unwrap() const -> T { return m_value; }

  operator T() const { return this->unwrap(); }

  /**
   * Set the value of a term to the *current* value of a formula.
   * @param form The formula to be evaluated and set this term to.
   */
  auto set(const formula<T>& form) -> void
  {
    T old_value = m_value;
    m_value = form.eval();
    // Nothing changed, early exit.
    if (old_value == m_value) {
      return;
    }
    // Make changes now that change happened.
    change_detected(old_value);
  }

  /**
   * Set the value of a term to the *current* value of a term.
   * @param the_term The term to evaluate and set this term to.
   */
  auto set(const term<T>& the_term) -> void
  {
    T old_value = m_value;
    m_value = the_term.unwrap();
    // Nothing changed, early exit.
    if (old_value == m_value) {
      return;
    }
    // Make changes now that change happened.
    change_detected(old_value);
  }

  /**
   * Set the value of a term to a specified value (of the appropriate type.
   * @param value The value to set this term to.
   */
  auto set(const T value) -> void
  {
    T old_value = m_value;
    m_value = value;
    // Nothing changed, early exit.
    if (old_value == m_value) {
      return;
    }
    // Make changes now that change happened.
    change_detected(old_value);
  }

  REGISTER_INPLACE(+=)
  REGISTER_INPLACE(-=)
  REGISTER_INPLACE(*=)
  REGISTER_INPLACE(/=)
  REGISTER_INPLACE(%=)
  REGISTER_INPLACE(^=)
  REGISTER_INPLACE(&=)
  REGISTER_INPLACE(|=)

  /**
   * Overload increment.
   * @return A reference to this term with its value incremented by one.
   */
  auto operator++() -> term<T>&
  {
    T new_value = this->unwrap() + 1;
    this->set(new_value);
    return *this;
  }

  /**
   * Overload decrement.
   * @return A reference to this term with its value decremented by one.
   */
  auto operator--() -> term<T>&
  {
    T new_value = this->unwrap() - 1;
    this->set(new_value);
    return *this;
  }

  /**
   * Overload increment. Note: Returns the previous value of type `T` not
   * `term<T>`.
   */
  auto operator++(int)
  {
    T return_value = this->unwrap();
    ++(*this);
    return return_value;
  }

  /**
   * Overload decrement. Note: Returns the previous value of type `T` not
   * `term<T>`.
   */
  auto operator--(int)
  {
    T return_value = this->unwrap();
    --(*this);
    return return_value;
  }
};

/**
 * `const_` objects are constants lifted into the expression tree, e.g. the `1`
 * in `y = x + 1`. Unlike `term` objects, they are *not* meant to be created or
 * kept alive by the user. They are heap allocated by the `REGISTER_*_OP(...)`
 * operator overloads and deallocate themselves once they no longer have a
 * parent formula (i.e. when the formula that used them is destroyed).
 */
template<typename T>
class const_
{
public:
  /** Do not touch directly! Needs to be public for user to use
   * REGISTER_*_OP(...) */
  std::vector<formula<T>*> m_parents;

  explicit const_(T value)
      : m_parents({})
      , m_value(value)
  {
  }

  template<typename U>
  auto remove_parent(formula<U>* parent) -> void
  {
    m_parents.erase(std::remove(m_parents.begin(), m_parents.end(), parent));
  }

  /**
   * Get the current value of the constant. (For `term` and `formula` use
   * `unwrap`/`eval` respectively.)
   */
  auto unwrap() const -> T { return m_value; }

  operator T() const { return this->unwrap(); }

private:
  T m_value;
};

/**
 * A "stmt" is either a constant, term or formula. Used extensively in `eval`
 * to determine how to evaluate an expression. (Note: Sister library `forcamla`
 * does *not* make this distinction.)
 */
template<typename T>
using stmt = std::variant<const_<T>*, term<T>*, formula<T>*>;

/**
 * Keep track of a unary operation applied to some inner formula (the "rhs").
 */
template<typename T>
struct unary_expr
{
  /** The operand. For instance, in the expression `!x`, `x` is `rhs`. */
  stmt<T> rhs;
  /** The operator. For instance, in the expression `!x`, `!` is `op`. */
  std::function<T(T)> op;
};

/**
 * Keep track of a binary operation applied to two (sub-)formula (the lhs and
 * rhs).
 */
template<typename T>
struct bin_expr
{
  /** The "left hand" operand. For instance, in the expression `x + y`, `x` is
   * `lhs`. */
  stmt<T> lhs;
  /** The "right hand" operand. For instance, in the expression `x + y`, `y` is
   * `rhs`. */
  stmt<T> rhs;
  /** The operator. For instance, in the expression `x + y`, `+` is `op`. */
  std::function<T(T, T)> op;
};

/**
 * `term` objects and `formula` objects are two of the fundamental types.
 * `formula` objects are `term` objects combined with operations to represent
 * a mathematical formula. They always evaluate to the most up-to-date value of
 * each `term` they use in their expression.
 */
template<typename T>
class formula
{
  bool m_needs_update;
  std::variant<unary_expr<T>, bin_expr<T>> m_expr;
  T m_cached_val;
  std::vector<stmt<T>> m_children;
  std::vector<std::function<void(T, T)>> m_on_change;
  /** True if this formula manages the expression tree it is part of. Copies
   * (e.g. from `auto z = x + y;`) are mere observers of the heap allocated
   * original and therefore do nothing when destroyed. */
  bool m_owns_tree;
  /** For observers: the heap allocated original formula. Since only the
   * original receives updates, observer listeners are registered there. Null
   * for originals. */
  formula<T>* m_original;

public:
  /** Do not touch directly! Needs to be public for user to use
   * REGISTER_*_OP(...) */
  std::vector<formula<T>*> m_parents;

  explicit formula(unary_expr<T> expr)
      : m_needs_update(true)
      , m_expr(expr)
      , m_cached_val(eval())
      , m_children({})
      , m_on_change({})
      , m_owns_tree(true)
      , m_original(nullptr)
      , m_parents({})
  {
    m_children.push_back(expr.rhs);
  }

  explicit formula(bin_expr<T> expr)
      : m_needs_update(true)
      , m_expr(expr)
      , m_cached_val(eval())
      , m_children({})
      , m_on_change({})
      , m_owns_tree(true)
      , m_original(nullptr)
      , m_parents({})
  {
    m_children.push_back(expr.lhs);
    m_children.push_back(expr.rhs);
  }

  /**
   * Copying a formula produces an observer of the original (heap allocated)
   * expression tree. The copy shares the same children/parents as the
   * original, but does *not* own them: destroying it leaves the tree intact.
   */
  formula(const formula<T>& other)
      : m_needs_update(other.m_needs_update)
      , m_expr(other.m_expr)
      , m_cached_val(other.m_cached_val)
      , m_children(other.m_children)
      , m_on_change(other.m_on_change)
      , m_owns_tree(false)
      , m_original(other.m_original != nullptr
                       ? other.m_original
                       // The observed formula is mutable in practice (it is
                       // the heap allocated original); the const here only
                       // comes from this copy constructor's signature.
                       : const_cast<formula<T>*>(&other))
      , m_parents(other.m_parents)
  {
  }

  formula<T>& operator=(const formula<T>& other) = delete;

  template<typename U>
  auto remove_parent(formula<U>* parent) -> void
  {
    m_parents.erase(std::remove(m_parents.begin(), m_parents.end(), parent));
  }

  /**
   * The heap allocated original of this formula: observers return their
   * original, originals return themselves. Operators use this to always build
   * on the canonical node, never on a stack bound observer copy.
   */
  auto original() -> formula<T>*
  {
    return m_original != nullptr ? m_original : this;
  }

  ~formula()
  {
    // Observers do not own the expression tree, so they must leave it alone.
    if (!m_owns_tree) {
      return;
    }
    // Mark parent formula for deletion. A copy of the parent list is used
    // since destroying a parent removes it from this list.
    auto parents = m_parents;
    for (auto* parent : parents) {
      delete parent;
    }
    for (auto child : m_children) {
      std::visit(overloaded {[this](term<T>* child) -> void
                             { child->remove_parent(this); },
                             [this](const_<T>* child) -> void
                             {
                               child->remove_parent(this);
                               // Constants are deallocated as soon as no
                               // formula uses them anymore.
                               if (child->m_parents.empty()) {
                                 delete child;
                               }
                             },
                             [this](formula<T>* child) -> void
                             { child->remove_parent(this); }},
                 child);
    }
  }

  /**
   * Indicate the formula's cached value is out of date and an update is needed
   */
  auto set_needs_update() -> void { m_needs_update = true; }

  /**
   * Get the current value of the formula.
   */
  auto eval() const -> T
  {
    if (!m_needs_update) {
      return m_cached_val;
    }
    return std::visit(
        overloaded {
            [](unary_expr<T> expression) -> T
            {
              auto inner = std::visit(
                  overloaded {[](const_<T>* val) -> T { return val->unwrap(); },
                              [](term<T>* val) -> T { return val->unwrap(); },
                              [](formula<T>* ast) -> T
                              {
                                return (ast->m_needs_update
                                            ? ast->eval()
                                            : ast->m_cached_val);
                              }},
                  expression.rhs);
              return expression.op(inner);
            },
            [](bin_expr<T> operand) -> T
            {
              auto inner_lhs = std::visit(
                  overloaded {[](const_<T>* val) -> T { return val->unwrap(); },
                              [](term<T>* val) -> T { return val->unwrap(); },
                              [](formula<T>* ast) -> T { return ast->eval(); }},
                  operand.lhs);
              auto inner_rhs = std::visit(
                  overloaded {[](const_<T>* val) -> T { return val->unwrap(); },
                              [](term<T>* val) -> T { return val->unwrap(); },
                              [](formula<T>* ast) -> T { return ast->eval(); }},
                  operand.rhs);
              return operand.op(inner_lhs, inner_rhs);
            }},
        m_expr);
  }

  operator T() const { return this->eval(); }

  /**
   * Update the cached value of the formula and also perform function calls
   * if the value was changed.
   */
  auto update() -> void
  {
    T old_val = m_cached_val;
    m_cached_val = eval();
    if (old_val != m_cached_val) {
      for (const auto& func : m_on_change) {
        func(old_val, m_cached_val);
      }
      m_needs_update = false;
      for (auto& parent : m_parents) {
        parent->set_needs_update();
        parent->update();
      }
    }
  }

  /**
   * Execute a function when the value of this formula changes.
   * @param func The function to execute. Takes the old and current value of
   * the formula. (The user is free to do with the old and current values as
   * they wish.)
   */
  auto on_change(std::function<void(T, T)> func) -> void
  {
    // Observers never receive updates themselves, only the original does.
    // Registering a listener on an observer therefore registers it on the
    // original, so that it is actually called.
    if (m_original != nullptr) {
      m_original->on_change(func);
      return;
    }
    m_on_change.push_back(func);
  }
};

/**
 * The operand node an operator should build upon. Terms are used as is, while
 * formulae are replaced by their heap allocated original (see
 * formula::original), so that operators applied to `auto` bound copies are
 * wired into the tree the copy observes.
 */
template<typename T>
auto node_of(term<T>& t) -> term<T>*
{
  return &t;
}

template<typename T>
auto node_of(formula<T>& form) -> formula<T>*
{
  return form.original();
}

REGISTER_BIN_OP(+)
REGISTER_BIN_OP(-)
REGISTER_BIN_OP(*)
REGISTER_BIN_OP(/)
REGISTER_BIN_OP(%)
REGISTER_BIN_OP(^)
REGISTER_BIN_OP(&)
REGISTER_BIN_OP(|)
REGISTER_BIN_OP_BOOL(&&)
REGISTER_BIN_OP_BOOL(||)

REGISTER_UNARY_OP(+)
REGISTER_UNARY_OP(-)
REGISTER_UNARY_OP(~)
REGISTER_UNARY_OP_BOOL(!)
