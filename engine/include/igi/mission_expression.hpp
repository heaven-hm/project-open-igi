#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace igi::gameplay {

// Evaluates boolean and numeric expressions from vanilla QSC scripts and authored mission objectives.
// Follows retail IGI1 expression syntax and operator precedence:
// OR (||), AND (&&), Equality (==, !=), Relational (<, <=, >, >=),
// Additive (+, -), Multiplicative (*, /), Unary (!, -, +), Primary (parentheses, numbers, variables).
class MissionExpressionState final {
public:
    using VariableResolver = std::function<bool(std::string_view name, double& out_value)>;

    void set_boolean(const std::string& variable_name, bool value);
    void set_number(const std::string& variable_name, double value);
    void clear() noexcept;

    [[nodiscard]] bool has_variable(const std::string& variable_name) const noexcept;
    [[nodiscard]] double get_number(const std::string& variable_name, double default_value = 0.0) const noexcept;
    [[nodiscard]] bool get_boolean(const std::string& variable_name, bool default_value = false) const noexcept;

    // Evaluates a boolean expression. Returns false if parsing fails or an unknown variable is referenced.
    [[nodiscard]] bool try_evaluate_boolean(
        const std::string& expression,
        bool& out_result,
        const VariableResolver& custom_resolver = {}) const;

    // Evaluates a numeric expression. Boolean subexpressions evaluate to 1.0 (true) or 0.0 (false).
    [[nodiscard]] bool try_evaluate_number(
        const std::string& expression,
        double& out_result,
        const VariableResolver& custom_resolver = {}) const;

private:
    std::unordered_map<std::string, double> variables_;
};

} // namespace igi::gameplay
