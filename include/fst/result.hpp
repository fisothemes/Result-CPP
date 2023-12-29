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

std::ostream& operator<<(std::ostream& os, const result_state& state) {
  switch (state) {
    case result_state::empty:
      return os << "empty";
    case result_state::success:
      return os << "success";
    case result_state::error:
      return os << "error";
    default:
      return os << "unknown";
  }
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
  const char* what() const noexcept override { return m_reason; }

 private:
  const char* m_reason = "bad result access";
};

template <typename T, typename E>
class result final {
 public:
  // Default constructor, creates an empty result
  constexpr result() : m_state(result_state::empty) {}

  /**
   * @brief Constructor for a successful result with a value.
   *
   * @tparam T The type of the success value.
   * @param value The success value to be stored.
   */
  template <typename U = T, std::enable_if<!std::is_same_v<U, E>>* = nullptr>
  constexpr result(const T& value)
      : m_state(result_state::success), m_value(value) {}

  /**
   * @brief Constructor for a failed result with an error value.
   *
   * @tparam E The type of the error value.
   * @param error The error value to be stored.
   */
  template <typename U = E, std::enable_if<!std::is_same_v<T, U>>* = nullptr>
  constexpr result(const E& error)
      : m_state(result_state::error), m_error(error) {}

  /**
   * @brief Constructor for a result with a specified state and value.
   *
   * @tparam T The type of the value.
   * @param state The state of the result (success or error).
   * @param value The success value to be stored if the state is success.
   */
  constexpr result(const result_state& state, const T& value) : m_state(state) {
    switch (state) {
      case result_state::success:
        m_value = value;
        break;

      case result_state::error:
        m_error = value;
        break;
    }
  }

    /**
   * @brief Constructor for a result with a specified state and value.
   *
   * @tparam E The type of the error value.
   * @param state The state of the result (success or error).
   * @param value The success value to be stored if the state is success.
   */
  template <typename U = E, std::enable_if<!std::is_same_v<T, U>>* = nullptr>
  constexpr result(const result_state& state, const E& value) : m_state(state) {
    switch (state) {
      case result_state::error:
        m_error = value;
        break;
    }
  }

  // move constructor.
  result(result<T, E>&& res) : m_state(res.state()) {
    switch (m_state) {
      case result_state::success:
        m_value = std::move(res.value());
        break;
      case result_state::error:
        m_error = std::move(*res.error());
        break;
      case result_state::empty:
        break;
      default:
        throw bad_result_access("Invalid state in move constructor");
    }
  }

  ~result() {
    switch (m_state) {
      case result_state::success:
        m_value.~T();
        break;
      case result_state::error:
        m_error.~E();
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
    return m_state == result_state::success ? std::optional<T>(m_value)
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
    return m_state == result_state::error ? std::optional<E>(m_error)
                                          : std::nullopt;
  }

  /**
   * @brief Retrieves the success value if the result is in a success state.
   * @tparam T Type of the success value.
   * @tparam E Type of the error value.
   * @return The const reference to success value.
   * @throw fst::bad_result_access If the result is not in a success state.
   */
  [[nodiscard]] constexpr const T& value() const {
    return m_state == result_state::success
               ? m_value
               : throw bad_result_access("Invalid state for value access");
  }

  /**
   * @brief Retrieves the success value if the result is in a success state;
   * otherwise, returns a default value.
   * @tparam T Type of the success value.
   * @tparam E Type of the error value.
   * @param default_value The value to return if the result is in an error
   * state.
   * @return A const reference to the success value if available; otherwise, the
   * specified default value.
   */
  [[nodiscard]] constexpr const T value_or(const T& default_value) const {
    return m_state == result_state::success ? m_value : default_value;
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
  [[nodiscard]] constexpr const T& expect(std::string& message) const {
    return m_state == result_state::success ? m_value
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
   * @brief Combines two results and returns the success result or the provided
   * result.
   *
   * This function is used to combine two results. If the current result is in a
   * success state, it creates and returns a new result<T, U> with the success
   * value of the current result. If the current result is in an error state, it
   * returns the provided result 'res' without modification.
   *
   * @tparam T Type of the success value in the current result.
   * @tparam E Type of the error value in the current result.
   * @tparam U Type of the success value in the provided result.
   * @param res The result to be returned if the current result is in an error
   * state.
   * @return The success result if the current result is in a success state;
   * otherwise, returns the provided result.
   */
  template <typename U>
  [[nodiscard]] constexpr const result<T, U> either(result<T, U>& res) const {
    return m_state == result_state::success ? result<T, U>(value()) : res;
  }

  [[nodiscard]] constexpr const result<T, E> or_else(
      result<T, E> f(const E& error)) const {
    return m_state == result_state::success
               ? result<T, E>(result_state::success, m_value)
               : f(m_error);
  }

  [[nodiscard]] constexpr const result<T, E> or_else(result<T, E> f(E& error)) {
    return m_state == result_state::success
               ? result<T, E>(result_state::success, m_value)
               : f(m_error);
  }

  [[nodiscard]] constexpr const result<T, E> and_then(
      result<T, E> f(const T& value)) const {
    return m_state == result_state::success
               ? f(m_value)
               : result<T, E>(result_state::error, m_error);
  }

  explicit operator bool() const { return m_state == result_state::success; }

  const T& operator*() const { return value(); }

  template <typename U>
  constexpr operator result<U, E>() const {
    return result<U, E>(U(value()));
  }

  template <typename U>
  constexpr operator result<T, U>() const {
    return result<T, U>(U(*error()));
  }

  friend std::ostream& operator<<(std::ostream& os, const result<T, E>& res) {
    switch (res.state()) {
      case result_state::success:
        return os << res.value();

      case result_state::error:
        return os << *res.error();

      default:
        throw bad_result_access();
    }
  };

 private:
  result_state m_state;
  union {
    T m_value;
    E m_error;
  };
};

}  // namespace fst

#endif  // FST_RESULT_HPP