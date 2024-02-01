#pragma once

#include <map>

template <typename T> class OrcaScope {
public:
  OrcaScope(OrcaScope *parent) : parent(parent) {
    if (parent == nullptr) {
      throw std::invalid_argument("Parent scope cannot be null.");
    }
  }
  OrcaScope() : parent(nullptr) {}

  ~OrcaScope() = default;

  /**
   * @brief Get the value of a symbol in the current scope or any parent scope.
   *
   * @param name The name of the symbol to get.
   * @return T* A pointer to the value of the symbol, or nullptr if the symbol
   *             does not exist.
   */
  T *get(const std::string &name) {
    if (symbols.find(name) != symbols.end()) {
      return &symbols[name];
    } else if (parent != nullptr) {
      return parent->get(name);
    } else {
      return nullptr;
    }
  }

  /**
   * @brief Set the value of a symbol in the current scope.
   *
   * @param name The name of the symbol to set.
   * @param value The value to set the symbol to.
   */
  void set(const std::string &name, const T &value) { symbols[name] = value; }

  /**
   * @brief Check if a symbol exists in the immediate scope.
   *
   * @param name The name of the symbol to check.
   * @return true If the symbol exists.
   * @return false If the symbol does not exist.
   */
  bool isInImmediateScope(const std::string &name) {
    return symbols.find(name) != symbols.end();
  }

  /**
   * @brief Check if a symbol exists in the current scope or any parent scope.
   *
   * @param name The name of the symbol to check.
   * @return true If the symbol exists.
   * @return false If the symbol does not exist.
   */
  bool isInScope(const std::string &name) { return get(name) != nullptr; }

  /**
   * @brief Get the parent scope.
   *
   * @return OrcaScope* A pointer to the parent scope.
   */
  OrcaScope<T> *getParent() { return parent; }

private:
  OrcaScope *parent;
  std::map<std::string, T> symbols;
};