/*
 *  Distributed under the MIT License (See accompanying file /LICENSE )
 */
#ifndef CALC_HPP
#define CALC_HPP

 // MS compatible compilers support #pragma once
#if defined(_MSC_VER)
#pragma once
#endif

#include <list>
#include <map>
#include <ostream>
#include <typeinfo>
#include "calc_step.h"
#include "logger.h"

namespace header_only_lib {

	template <class Type>
	using Dictionary = std::map<std::string, Type>;

	namespace DefaultOperations {
		const Operation Plus = [](const int &value1, const int &value2) {
			return value1 + value2;
		};
		const Operation Minus = [](const int &value1, const int &value2) {
			return value1 - value2;
		};
		const Operation Times = [](const int &value1, const int &value2) {
			return value1 * value2;
		};
		const Operation Div = [](const int &value1, const int &value2) {
			return value1 / value2;
		};
		const Operation Zero = [](const int &value1, const int &value2) { return 0; };
	}  // namespace DefaultOperations

	class Calc {
	public:
		Calc() noexcept {};

		Calc(const Calc &other) noexcept
			: operations_{ other.operations_ }, steps_{ other.steps_ } {}

		Calc &Calc::operator=(const Calc &other) noexcept {
			operations_ = { other.operations_ };
			steps_ = { other.steps_ };

			return *this;
		}

		auto total_operations() const noexcept { return operations_.size(); }

		void Calc::add_step(const CalcStep &step) { steps_.push_back(step); }

		void Calc::add_step(const int value) { add_step(CalcStep{ value }); }

		void Calc::add_step(const std::string &operation_name) {
			auto &operation = operations_[operation_name];

			if (operation == nullptr) {
				operation = DefaultOperations::Zero;
			}

			add_step(CalcStep{ operation_name, operation });
		}

		auto total_steps() const noexcept { return steps_.size(); }

		int Calc::result() const {
			logger_.debug("[{}]", __func__);

			int total{ 0 };
			logger_.trace("total is {}", total);
			Operation operation_to_execute{ DefaultOperations::Zero };
			bool execute_operation{ false };
			bool first_value{ true };

			for (const auto &step : steps_) {
				logger_.trace("step = {}", step);
				if (step.has_operation()) {
					logger_.trace("step is an operation");
					operation_to_execute = step.operation();
					execute_operation = true;
					continue;
				}

				if (step.has_value()) {
					logger_.trace("step has value");
					if (first_value) {
						logger_.trace("step is first step");
						total = step.value();
						first_value = false;
						continue;
					}
					if (execute_operation) {
						logger_.trace("executing operation");
						total = operation_to_execute(total, step.value());
						execute_operation = false;
						logger_.trace("total so far is = {}", total);
					}
				}
			}

			logger_.debug("[{}] = {}", __func__, total);

			return total;
		}

		template <typename T>
		Calc operator<<(const T &value) const {
			Calc new_calc{ *this };
			new_calc.add_step(value);
			return new_calc;
		}

		friend std::ostream &operator<<(std::ostream &stream, const Calc &calc) {
			for (const auto &step : calc.steps_) {
				stream << step << " ";
			}

			stream << "= " << calc.result();
			return stream;
		}

	private:
		std::list<CalcStep> steps_{};
		Dictionary<Operation> operations_{{"+", DefaultOperations::Plus},
										  {"-", DefaultOperations::Minus},
										  {"*", DefaultOperations::Times},
										  {"/", DefaultOperations::Div} };
		static Logger logger_;
	};

	Logger Calc::logger_{ typeid(Calc).name() };

}  // namespace ModernCppCI

#endif  // CALC_HPP