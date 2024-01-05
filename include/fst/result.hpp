// Result.h
#ifndef FST_RESULT_HPP
#define FST_RESULT_HPP

#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace fst {

/**
 * @brief Enum representing the state of a result object (empty for an
 * uninitialised result, success for a successful result and error for a
 * failed).
 */
enum class result_state : unsigned char { empty, success, error };

// Enum to represent the success state tag.
enum class success_tag : unsigned char { success };

// Enum to represent the error state tag.
enum class error_tag : unsigned char { error };

// Enum to represent the empty state tag.
enum class empty_tag : unsigned char { empty };

// Alias for the success tag value.
constexpr success_tag success_t = success_tag::success;

// Alias for the error tag value.
constexpr error_tag error_t = error_tag::error;

// Alias for the empty tag value.
constexpr empty_tag empty_t = empty_tag::empty;

/**
 * @brief Converts a result_state enum to a string.
 *
 * @param state The result_state to convert.
 * @return A string representation of the result_state.
 */
std::string to_string(const result_state& state) {
  switch (state) {
    case result_state::empty:
      return "empty";
    case result_state::success:
      return "success";
    case result_state::error:
      return "error";
    default:
      return "unknown";
  }
}

/**
 * @brief Converts a result_state enum to a string and streams it to an output
 * stream.
 *
 * @param os The output stream to write to.
 * @param state The result_state to convert and stream.
 * @return The modified output stream.
 */
std::ostream& operator<<(std::ostream& os, const result_state& state) {
  return os << to_string(state);
}

/**
 * @brief Exception class for indicating invalid access to a result object.
 *
 * This exception is thrown when attempting to access the value or error of a
 * result object in an invalid state, such as when trying to access the success
 * value of an error result or vice versa.
 */
class bad_result_access : public std::exception {
 public:
  bad_result_access() noexcept {}
  bad_result_access(const char* reason) noexcept : m_reason(reason) {}
  bad_result_access(const std::string& reason) noexcept
      : m_reason(reason.c_str()) {}
  const char* what() const noexcept override { return m_reason; }

 private:
  const char* m_reason = "bad result access";
};

/**
 * @brief Generic class that implements the monadic pattern for error handling.
 * It can store either a successful value of type T or an error value of type E.
 * The class includes methods for extracting the success or error values safely
 * and handling different outcomes through chaining operations.
 *
 * @tparam T Type of the success value.
 * @tparam E Type of the error value.
 */
template <typename T, typename E>
class result final {
 public:
  using value_type = T;
  using error_type = E;

  // Default constructor, creates an empty result
  constexpr result() : m_state(result_state::empty), m_self(success_t, T{}) {}

  /**
   * @brief Constructor for a successful result with a value.
   *
   * @tparam T The type of the success value.
   * @param value The success value to be stored.
   */
  template <typename U = T, std::enable_if<!std::is_same_v<U, E>>* = nullptr>
  constexpr result(const T& value)
      : m_state(result_state::success), m_self(success_t, value) {}

  /**
   * @brief Constructor for a failed result with an error value.
   *
   * @tparam E The type of the error value.
   * @param error The error value to be stored.
   */
  template <typename U = E, std::enable_if<!std::is_same_v<T, U>>* = nullptr>
  constexpr result(const E& error)
      : m_state(result_state::error), m_self(error_t, error) {}

  /**
   * @brief Constructor for a successful result with a value, using a success
   * tag.
   *
   * This constructor is particularly useful in cases where the type of the
   * success value (T) is the same as the type of the error value (E) or when
   * there might be ambiguity in type conversion.
   *
   * @tparam T The type of the success value.
   * @param tag The success tag, indicating a successful result.
   * @param value The success value to be stored.
   */
  constexpr result(success_tag tag, const T& value)
      : m_state(result_state::success), m_self(success_t, value) {}

  /**
   * @brief Constructor for a failed result with an error value, using an error
   * tag.
   *
   * This constructor is particularly useful in cases where the type of the
   * success value (T) is the same as the type of the error value (E) or when
   * there might be ambiguity in type conversion.
   *
   * @tparam E The type of the error value.
   * @param tag The error tag, indicating an error result.
   * @param error The error value to be stored.
   */
  constexpr result(error_tag tag, const E& error)
      : m_state(result_state::error), m_self(error_t, error) {}

  result(result<T, E>& res) : m_state(res.state()) {
    switch (res.state()) {
      case result_state::success:
        m_self.m_value = res.m_self.m_value;
        break;
      case result_state::error:
        m_self.m_error = res.m_self.m_error;
        break;
      case result_state::empty:
        break;
      default:
        throw bad_result_access("Invalid state in copy constructor");
    }
  }

  result(result<T, E>&& res) : m_state(res.state()) {
    switch (res.m_state) {
      case result_state::success:
        m_self.m_value = std::move(res.m_self.m_value);
        break;
      case result_state::error:
        m_self.m_error = std::move(res.m_self.m_error);
        break;
      case result_state::empty:
        break;
      default:
        throw bad_result_access("Invalid state in move constructor");
    }
    res.m_state = result_state::empty;
  }

  ~result() {
    std::cout << "<<DESTROYED>>: " << *this << " @ STATE: " << m_state << '\n';
    switch (m_state) {
      case result_state::success:
        m_self.m_value.~T();
        break;
      case result_state::error:
        m_self.m_error.~E();
        break;
      default:
        break;
    }
  }

  /**
   * @brief Retrieves the success value if the result is in a success state.
   * @tparam T Type of the success value.
   * @tparam E Type of the error value.
   * @return An optional containing the success value if available, otherwise
   * std::nullopt.
   */
  [[nodiscard]] constexpr const std::optional<T> success() const {
    return m_state == result_state::success ? std::optional<T>(m_self.m_value)
                                            : std::nullopt;
  }

  /**
   * @brief Retrieves the error value if the result is in an error state.
   * @tparam T Type of the success value.
   * @tparam E Type of the error value.
   * @return An optional containing the error value if available, otherwise
   * std::nullopt.
   */
  [[nodiscard]] constexpr const std::optional<E> error() const {
    return m_state == result_state::error ? std::optional<E>(m_self.m_error)
                                          : std::nullopt;
  }

  /**
   * @brief Retrieves the success value of the result.
   * @tparam T Type of the success value.
   * @tparam E Type of the error value.
   * @return The const reference to success value.
   * @throw std::bad_result_access if result does not contain a success value.
   */
  [[nodiscard]] constexpr const T& value() const {
    return m_state == result_state::success
               ? m_self.m_value
               : throw bad_result_access(
                     "Invalid state for value access, result's state was: " +
                     to_string(m_state));
  }

  /**
   * @brief Retrieves the success value of the result; otherwise, returns a
   * default value.
   * @tparam T Type of the success value.
   * @tparam E Type of the error value.
   * @param default_value The value to return if the result is in an error
   * state.
   * @return The success value if available; otherwise, the
   * specified default value.
   */
  [[nodiscard]] constexpr const T value_or(const T& default_value = T{}) const {
    return m_state == result_state::success ? m_self.m_value : default_value;
  }

  /**
   * @brief Retrieves the success value if the result is in a success state.
   * @tparam T Type of the success value.
   * @tparam E Type of the error value.
   * @param message A message to include in the exception if the result is in an
   * error state.
   * @return A const reference to the success value.
   * @throw std::runtime_error If the result is not in a success state,
   * including the specified message.
   */
  [[nodiscard]] constexpr const T& expect(const std::string& message) const {
    return m_state == result_state::success ? m_self.m_value
                                            : throw std::runtime_error(message);
  }

  /**
   * @brief Retrieves the state of the result (success or error).
   * @tparam T Type of the success value.
   * @tparam E Type of the error value.
   * @return The state of the result.
   */
  [[nodiscard]] constexpr const result_state& state() const { return m_state; }

  /**
   * @brief Checks if the result contains a success value.
   * @tparam T Type of the success value.
   * @tparam E Type of the error value.
   * @return True if the result is in a success state; otherwise, false.
   */
  [[nodiscard]] constexpr bool has_value() const {
    return m_state == result_state::success;
  }

  /**
   * @brief Checks if the result contains an error value.
   * @tparam T Type of the success value.
   * @tparam E Type of the error value.
   * @return True if the result is in an error state; otherwise, false.
   */
  [[nodiscard]] constexpr bool has_error() const {
    return m_state == result_state::error;
  }

  /**
   * @brief Checks if the result is empty.
   * @tparam T Type of the success value.
   * @tparam E Type of the error value.
   * @return True if the result is empty; otherwise, false.
   */
  [[nodiscard]] constexpr bool is_empty() const {
    return m_state == result_state::empty;
  }

  /**
   * @brief Applies the provided function to the error value if the result is
   * in an error state, returning a new result.
   *
   * If the result is in a success state, returns the original success result.
   * If the result is in an error state, applies the provided callable function
   * to the error value and returns the result.
   *
   * @tparam F Type of the callable function.
   * @param f Callable function to be applied to the error value if the result
   * is in an error state.
   * @return The original success result or the result of applying the callable
   * function to the error value.
   *
   * @note The provided callable function must have the signature:
   *       `auto lambda(const E& error) -> result<T, U>`,
   *       `auto lambda(const auto& error) -> result<T, U>`,
   *       `auto lambda(E error) -> result<T, U>` or
   *       `auto lambda(auto error) -> result<T, U>`
   *       where T is the original success value type and U is the desired error
   *       value type.
   */
  template <typename F>
  constexpr auto or_else(F&& f) const&
      -> result<T, typename decltype(f(*error()))::error_type> {
    return m_state == result_state::error
               ? f(m_self.m_error)
               : decltype(f(*error()))(success_t, value_or());
  }

  /**
   * @brief Applies the provided callable function to the success value if the
   * result is in a success state, returning a new result.
   *
   * If the result is in an error state, returns the original error result.
   * If the result is in a success state, applies the provided callable function
   * to the success value and returns the result.
   *
   * @tparam F Type of the callable function.
   * @param f Callable function to be applied to the success value if the result
   * is in a success state.
   * @return The original error result or the result of applying the callable
   * function to the success value.
   *
   * @note The provided callable function must have the following signatures:
   *       `auto func(const T& value) -> result<U, E>`,
   *       `auto func(const auto& value) -> result<U, E>`,
   *       `auto func(T value) -> result<U, E>`, or
   *       `auto func(auto value) -> result<U, E>`
   *       where U is the desired success value type and E is the original error
   *       value type.
   */
  template <typename F>
  constexpr auto and_then(F&& f) const&
      -> result<typename decltype(f(*success()))::value_type, E> {
    return m_state == result_state::success
               ? f(m_self.m_value)
               : decltype(f(*success()))(error_t, m_self.m_error);
  }

  /**
   * @brief Maps the success value using the provided function.
   *
   * If the result is in a success state, applies the provided function `f` to
   * the success value, modifying it in place.
   *
   * @tparam T Type of the success value.
   * @tparam E Type of the error value.
   * @param f Callable function to be applied to the success value if the result
   * is in a success state.
   * @return Reference to the modified current result.
   *
   * @note The provided callable function must have the following signatures:
   *       `auto f(const T& value) -> T` or
   *       `auto f(const auto& value) -> T`
   *       where T is the type of the success value.
   */
  template <typename F>
  constexpr auto map(F&& f) {
    return m_state == result_state::success
               ? result<T, E>(success_t, f(m_self.m_value))
           : m_state == result_state::error
               ? result<T, E>(error_t, m_self.m_error)
               : result<T, E>();
  }

  /**
   * @brief Maps the error value using the provided function.
   *
   * If the result is in an error state, applies the provided function `f` to
   * the error value, modifying it in place.
   *
   * @tparam T Type of the success value.
   * @tparam E Type of the error value.
   * @param f Callable function to be applied to the error value if the result
   * is in an error state.
   * @return Reference to the modified current result.
   *
   * @note The provided callable function must have the following signatures:
   *       `f(const E& error) -> E` or
   *       `f(const auto& error) -> E`
   *       where E is the type of the error value.
   */
  template <typename F>
  constexpr auto map_error(F&& f) {
    return m_state == result_state::error
               ? result<T, E>(error_t, f(m_self.m_error))
           : m_state == result_state::success
               ? result<T, E>(success_t, m_self.m_value)
               : result<T, E>();
  }

  /**
   * @brief Transforms the result using the provided callable function.
   *
   * Applies the callable function to the current result, producing a
   * new result based on the transformation. The transformation function is
   * expected to take the current result as an argument and return a new result.
   *
   * @tparam F Type of the callable function.
   * @param f Callable function to transform the current result.
   * @return The result of applying the transformation function to the current
   * result.
   *
   * @note The provided callable function must have the signatures:
   *       `func(const result<T, E>& res) -> result<U, V>` or
   *       `func(const auto& res) -> result<U, V>`,
   *       where U is the desired success value type and V is the desired error
   *       value type for the transformed result.
   */
  template <typename F>
  constexpr auto transform(F&& f) const&
      -> result<typename decltype(f(*this))::value_type,
                typename decltype(f(*this))::error_type> {
    return f(*this);
    ;
  }

  /**
   * @brief Invokes a callable function on the current result without modifying
   * it.
   *
   * This function calls the provided function `f` with a constant reference to
   * the current result without modifying it. It is designed for cases where
   * side effects are intended without altering the result.
   *
   * @param f A function that takes a constant reference to the result.
   *          The function should have the signatures:
   *          `void f(const result<T,E>& res)` or
   *          `void f(const auto& res)`.
   *
   * @return The unmodified current result after invoking the function.
   */

  template <typename F>
  constexpr result<T, E> inspect(F&& f) const& {
    auto res = m_state == result_state::success
               ? result<T, E>(success_t, m_self.m_value)
           : m_state == result_state::error
               ? result<T, E>(error_t, m_self.m_error)
               : result<T, E>();

    f(res);
    return res;
  }

  /**
   * @brief Explicit conversion to bool.
   * @return True if the result is in a success state; otherwise, false.
   */
  explicit operator bool() const { return m_state == result_state::success; }

  /**
   * @brief Dereference operator.
   * @return Const reference to the success value.
   * @throw bad_result_access If the result is not in a success state.
   */
  const T& operator*() const { return *success(); }

  /**
   * @brief Converts the result to a different result type, mapping the success
   * value with the provided conversion function.
   * @tparam U Type of the success value in the new result.
   * @tparam E Type of the error value in the original result.
   * @return A new result with the success value converted to type U.
   */
  template <typename U>
  constexpr operator result<U, E>() const {
    return result<U, E>(U(value()));
  }

  /**
   * @brief Converts the result to a different result type, mapping the error
   * value with the provided conversion function.
   * @tparam T Type of the success value in the original result.
   * @tparam U Type of the error value in the new result.
   * @return A new result with the error value converted to type E.
   */
  template <typename U>
  constexpr operator result<T, U>() const {
    return result<T, U>(U(*error()));
  }

  friend std::ostream& operator<<(std::ostream& os, const result<T, E>& res) {
    switch (res.state()) {
      case result_state::success:
        return os << *res.success();

      case result_state::error:
        return os << *res.error();

      case result_state::empty:
        return os;

      default:
        throw bad_result_access();
    }
  }

 private:
  result_state m_state = result_state::empty;
  union members {
    T m_value;
    E m_error;
    members() {}
    members(success_tag tag, T val) : m_value(val) {}
    members(error_tag tag, E err) : m_error(err) {}
    ~members() {}
  } m_self;
};

}  // namespace fst

#endif  // FST_RESULT_HPP