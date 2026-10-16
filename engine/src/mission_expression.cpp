#include "igi/mission_expression.hpp"
#include "igi/logger.hpp"

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <string_view>
#include <utility>

namespace igi::gameplay {

namespace {

class ExpressionParser final {
public:
    ExpressionParser(
        std::string_view expression,
        MissionExpressionState::VariableResolver resolver)
        : expression_(expression), resolver_(std::move(resolver)) {}

    [[nodiscard]] bool try_parse_number(double& out_result) {
        if (!parse_or(out_result)) {
            return false;
        }
        skip_whitespace();
        return cursor_ == expression_.size();
    }

private:
    [[nodiscard]] static bool is_truthy(double value) noexcept {
        return std::fabs(value) > 1.0e-7;
    }

    void skip_whitespace() noexcept {
        while (cursor_ < expression_.size() &&
               std::isspace(static_cast<unsigned char>(expression_[cursor_]))) {
            ++cursor_;
        }
    }

    [[nodiscard]] bool match(std::string_view token) noexcept {
        skip_whitespace();
        if (expression_.substr(cursor_, token.size()) != token) {
            return false;
        }
        cursor_ += token.size();
        return true;
    }

    [[nodiscard]] bool parse_or(double& out_value) {
        if (!parse_and(out_value)) {
            return false;
        }
        while (match("||")) {
            double right_value = 0.0;
            if (!parse_and(right_value)) {
                return false;
            }
            out_value = (is_truthy(out_value) || is_truthy(right_value)) ? 1.0 : 0.0;
        }
        return true;
    }

    [[nodiscard]] bool parse_and(double& out_value) {
        if (!parse_equality(out_value)) {
            return false;
        }
        while (match("&&")) {
            double right_value = 0.0;
            if (!parse_equality(right_value)) {
                return false;
            }
            out_value = (is_truthy(out_value) && is_truthy(right_value)) ? 1.0 : 0.0;
        }
        return true;
    }

    [[nodiscard]] bool parse_equality(double& out_value) {
        if (!parse_relational(out_value)) {
            return false;
        }
        while (true) {
            const bool equals = match("==");
            const bool not_equals = !equals && match("!=");
            if (!equals && !not_equals) {
                return true;
            }

            double right_value = 0.0;
            if (!parse_relational(right_value)) {
                return false;
            }
            const bool comparison = equals
                ? (std::fabs(out_value - right_value) <= 1.0e-7)
                : (std::fabs(out_value - right_value) > 1.0e-7);
            out_value = comparison ? 1.0 : 0.0;
        }
    }

    [[nodiscard]] bool parse_relational(double& out_value) {
        if (!parse_additive(out_value)) {
            return false;
        }
        while (true) {
            enum class RelOp { None, Less, LessOrEqual, Greater, GreaterOrEqual };
            RelOp op = RelOp::None;
            if (match("<=")) {
                op = RelOp::LessOrEqual;
            } else if (match(">=")) {
                op = RelOp::GreaterOrEqual;
            } else if (match("<")) {
                op = RelOp::Less;
            } else if (match(">")) {
                op = RelOp::Greater;
            } else {
                return true;
            }

            double right_value = 0.0;
            if (!parse_additive(right_value)) {
                return false;
            }
            switch (op) {
                case RelOp::Less:
                    out_value = out_value < right_value ? 1.0 : 0.0;
                    break;
                case RelOp::LessOrEqual:
                    out_value = out_value <= right_value ? 1.0 : 0.0;
                    break;
                case RelOp::Greater:
                    out_value = out_value > right_value ? 1.0 : 0.0;
                    break;
                case RelOp::GreaterOrEqual:
                    out_value = out_value >= right_value ? 1.0 : 0.0;
                    break;
                case RelOp::None:
                    return false;
            }
        }
    }

    [[nodiscard]] bool parse_additive(double& out_value) {
        if (!parse_multiplicative(out_value)) {
            return false;
        }
        while (true) {
            const bool add = match("+");
            const bool subtract = !add && match("-");
            if (!add && !subtract) {
                return true;
            }

            double right_value = 0.0;
            if (!parse_multiplicative(right_value)) {
                return false;
            }
            out_value = add ? out_value + right_value : out_value - right_value;
        }
    }

    [[nodiscard]] bool parse_multiplicative(double& out_value) {
        if (!parse_unary(out_value)) {
            return false;
        }
        while (true) {
            const bool multiply = match("*");
            const bool divide = !multiply && match("/");
            if (!multiply && !divide) {
                return true;
            }

            double right_value = 0.0;
            if (!parse_unary(right_value)) {
                return false;
            }
            if (divide && std::fabs(right_value) <= 1.0e-7) {
                return false; // Division by zero rejected
            }
            out_value = multiply ? out_value * right_value : out_value / right_value;
        }
    }

    [[nodiscard]] bool parse_unary(double& out_value) {
        if (match("!")) {
            if (!parse_unary(out_value)) {
                return false;
            }
            out_value = is_truthy(out_value) ? 0.0 : 1.0;
            return true;
        }
        if (match("-")) {
            if (!parse_unary(out_value)) {
                return false;
            }
            out_value = -out_value;
            return true;
        }
        if (match("+")) {
            return parse_unary(out_value);
        }
        return parse_primary(out_value);
    }

    [[nodiscard]] bool parse_primary(double& out_value) {
        skip_whitespace();
        if (match("(")) {
            if (!parse_or(out_value) || !match(")")) {
                return false;
            }
            return true;
        }

        if (cursor_ >= expression_.size()) {
            return false;
        }

        const char current_char = expression_[cursor_];
        if (std::isdigit(static_cast<unsigned char>(current_char)) || current_char == '.') {
            return parse_number(out_value);
        }
        return parse_identifier(out_value);
    }

    [[nodiscard]] bool parse_number(double& out_value) {
        const char* start_ptr = expression_.data() + cursor_;
        char* end_ptr = nullptr;
        out_value = std::strtod(start_ptr, &end_ptr);
        if (end_ptr == start_ptr) {
            return false;
        }
        cursor_ = static_cast<std::size_t>(end_ptr - expression_.data());
        return std::isfinite(out_value);
    }

    [[nodiscard]] bool parse_identifier(double& out_value) {
        skip_whitespace();
        const std::size_t start = cursor_;
        while (cursor_ < expression_.size()) {
            const unsigned char c = static_cast<unsigned char>(expression_[cursor_]);
            if (!std::isalnum(c) && c != '_' && c != '.') {
                break;
            }
            ++cursor_;
        }
        if (start == cursor_) {
            return false;
        }

        const std::string_view identifier = expression_.substr(start, cursor_ - start);
        if (identifier == "TRUE") {
            out_value = 1.0;
            return true;
        }
        if (identifier == "FALSE") {
            out_value = 0.0;
            return true;
        }
        if (identifier == "GAME_FREQUENCY") {
            out_value = 30.0; // Retail IGI 30 Hz simulation clock
            return true;
        }

        if (resolver_) {
            return resolver_(identifier, out_value);
        }
        return false;
    }

    std::string_view expression_;
    MissionExpressionState::VariableResolver resolver_;
    std::size_t cursor_ = 0;
};

} // namespace

void MissionExpressionState::set_boolean(const std::string& variable_name, bool value) {
    variables_[variable_name] = value ? 1.0 : 0.0;
}

void MissionExpressionState::set_number(const std::string& variable_name, double value) {
    variables_[variable_name] = value;
}

void MissionExpressionState::clear() noexcept {
    variables_.clear();
}

bool MissionExpressionState::has_variable(const std::string& variable_name) const noexcept {
    return variables_.find(variable_name) != variables_.end();
}

double MissionExpressionState::get_number(const std::string& variable_name, double default_value) const noexcept {
    const auto it = variables_.find(variable_name);
    return it != variables_.end() ? it->second : default_value;
}

bool MissionExpressionState::get_boolean(const std::string& variable_name, bool default_value) const noexcept {
    const auto it = variables_.find(variable_name);
    return it != variables_.end() ? (std::fabs(it->second) > 1.0e-7) : default_value;
}

bool MissionExpressionState::try_evaluate_boolean(
    const std::string& expression,
    bool& out_result,
    const VariableResolver& custom_resolver) const {
    double numeric_val = 0.0;
    if (!try_evaluate_number(expression, numeric_val, custom_resolver)) {
        return false;
    }
    out_result = std::fabs(numeric_val) > 1.0e-7;
    return true;
}

bool MissionExpressionState::try_evaluate_number(
    const std::string& expression,
    double& out_result,
    const VariableResolver& custom_resolver) const {
    const VariableResolver combined_resolver = [this, &custom_resolver](
        std::string_view name, double& val) -> bool {
        if (custom_resolver && custom_resolver(name, val)) {
            return true;
        }
        const auto it = variables_.find(std::string(name));
        if (it != variables_.end()) {
            val = it->second;
            return true;
        }
        return false;
    };

    ExpressionParser parser(expression, combined_resolver);
    return parser.try_parse_number(out_result);
}

} // namespace igi::gameplay
