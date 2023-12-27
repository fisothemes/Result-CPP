// Result.h
#ifndef RESULT_HPP
#define RESULT_HPP

#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>

/** declarations ***********************************************************/
namespace fst {
enum class result_state { success, error };

std::ostream& operator<<(std::ostream& os, const result_state& state);

template <typename T, typename E>
class result final {
 public:
  constexpr result(const T& value);
  constexpr result(const result_state& state, const T& value);
  constexpr result(const E& error);
  constexpr result(const result_state& state, const E& error);

  [[nodiscard]] const std::optional<T> success() const;
  [[nodiscard]] const std::optional<E> error() const;

  [[nodiscard]] const T& value() const;
  [[nodiscard]] const T value_or(const T& default_value) const;

  [[nodiscard]] constexpr const T& expect(std::string& message) const;

  [[nodiscard]] constexpr const result_state state() const;

  template <typename U>
  const result<T, U> either(result<T, U>& res) const;

  template <typename U, typename F>
  const result<T, U> or_else(F f) const;

  template <typename U, typename F>
  const result<U, E> and_then(F f) const;

  explicit operator bool() const;

  const T& operator*() const;

  template <typename U>
  constexpr operator result<U, E>() const;

  template <typename U>
  constexpr operator result<T, U>() const;

  template <typename U, typename V>
  friend std::ostream& operator<<(std::ostream& os, const result<U, V>& res);

 private:
  std::variant<T, E> m_value = {};
};

}  // namespace fst

/** implementations *********************************************************/
namespace fst {

std::ostream& operator<<(std::ostream& os, const result_state& state) {
  switch (state) {
    case result_state::success:
      return os << "success";
    case result_state::error:
      return os << "error";
    default:
      return os << "unknown";
  }
}

/**
 * @brief Constructor for a successful result with a value.
 * @tparam T Type of the success value.
 * @tparam E Type of the error value.
 * @param value The success value.
 */
template <typename T, typename E>
constexpr result<T, E>::result(const T& value) : m_value(value) {}

/**
 * @brief Constructor for a result with a specified state and value.
 * @tparam T Type of the success value.
 * @tparam E Type of the error value.
 * @param state The state of the result (success or error).
 * @param value The value associated with the result.
 * @throw std::invalid_argument If the specified state is inconsistent with the
 * result's purpose.
 */
template <typename T, typename E>
constexpr result<T, E>::result(const result_state& state, const T& value)
    : m_value((state == result_state::success)
                  ? value
                  : throw std::invalid_argument(
                        "Invalid result state for success.")) {}

/**
 * @brief Constructor for a failed result with an error value.
 * @tparam T Type of the success value.
 * @tparam E Type of the error value.
 * @param error The error value.
 */
template <typename T, typename E>
constexpr result<T, E>::result(const E& error) : m_value(error) {}

/**
 * @brief Constructor for a result with a specified state and error value.
 * @tparam T Type of the success value.
 * @tparam E Type of the error value.
 * @param state The state of the result (success or error).
 * @param error The error value associated with the result.
 * @throw std::invalid_argument If the specified state is inconsistent with the
 * result's purpose.
 */
template <typename T, typename E>
constexpr result<T, E>::result(const result_state& state, const E& error)
    : m_value(state == result_state::success
                  ? error
                  : throw std::invalid_argument(
                        "Invalid result state for error.")) {}

/**
 * @brief Retrieves the success value if the result is in a success state.
 * @tparam T Type of the success value.
 * @tparam E Type of the error value.
 * @return An optional containing the success value if available, otherwise
 * std::nullopt.
 */
template <typename T, typename E>
[[nodiscard]] const std::optional<T> result<T, E>::success() const {
  if (auto val = std::get_if<0>(m_value)) return std::optional<T>(*val);
  return std::nullopt;
}

/**
 * @brief Retrieves the error value if the result is in an error state.
 * @tparam T Type of the success value.
 * @tparam E Type of the error value.
 * @return An optional containing the error value if available, otherwise
 * std::nullopt.
 */
template <typename T, typename E>
[[nodiscard]] const std::optional<E> result<T, E>::error() const {
  if (auto err = std::get_if<1>(&m_value)) return std::optional<E>(*err);
  return std::nullopt;
}

/**
 * @brief Retrieves the success value if the result is in a success state.
 * @tparam T Type of the success value.
 * @tparam E Type of the error value.
 * @return A const reference to the success value.
 * @throw std::bad_variant_access If the result is not in a success state.
 */
template <typename T, typename E>
[[nodiscard]] const T& result<T, E>::value() const {
  return std::get<0>(m_value);
}

/**
 * @brief Retrieves the success value if the result is in a success state;
 * otherwise, returns a default value.
 * @tparam T Type of the success value.
 * @tparam E Type of the error value.
 * @param default_value The value to return if the result is in an error state.
 * @return A const reference to the success value if available; otherwise, the
 * specified default value.
 */
template <typename T, typename E>
[[nodiscard]] const T result<T, E>::value_or(const T& default_value) const {
  if (auto val = std::get_if<0>(m_value)) return *val;
  return default_value;
}

/**
 * @brief Retrieves the success value if the result is in a success state.
 * @tparam T Type of the success value.
 * @tparam E Type of the error value.
 * @param message A message to include in the exception if the result is in an
 * error state.
 * @return A const reference to the success value.
 * @throw std::runtime_error If the result is not in a success state, including
 * the specified message.
 */
template <typename T, typename E>
[[nodiscard]] constexpr const T& result<T, E>::expect(std::string& message) const {
  if (auto val = std::get_if<0>(&m_value)) return *val;
  throw std::runtime_error(message);
}

/**
 * @brief Retrieves the state of the result (success or error).
 * @tparam T Type of the success value.
 * @tparam E Type of the error value.
 * @return The state of the result.
 */
template <typename T, typename E>
[[nodiscard]] constexpr const result_state result<T, E>::state() const {
  return std::get_if<0>(&m_value) ? result_state::success : result_state::error;
}

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
template <typename T, typename E>
template <typename U>
const result<T, U> result<T, E>::either(result<T, U>& res) const {
  return (state() == result_state::success) ? result<T, U>(value()) : res;
}

template <typename T, typename E>
result<T, E>::operator bool() const {
  return state() == result_state::success;
}

template <typename T, typename E>
const T& result<T, E>::operator*() const {
  value();
}

template <typename T, typename E>
template <typename U>
constexpr result<T, E>::operator result<U, E>() const {
  return result<U, E>(state() == result_state::success ? U(value())
                                                       : E(error()));
}

template <typename T, typename E>
template <typename U>
constexpr result<T, E>::operator result<T, U>() const {
  return result<T, U>(state() == result_state::error ? U(error()) : T(value()));
}

template <typename U, typename V>
std::ostream& operator<<(std::ostream& os, const result<U, V>& res) {
  return (res.state() == result_state::success)
             ? os << res.value()
             : os << "Error: " << res.error().value();
}

}  // namespace fst

#endif  // RESULT_HPP