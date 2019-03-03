/*
 *  Distributed under the MIT License (See accompanying file /LICENSE )
 */
#ifndef LOG_HPP
#define LOG_HPP

 // MS compatible compilers support #pragma once
#if defined(_MSC_VER)
#pragma once
#endif

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <string>

#include <spdlog/sinks/dist_sink.h>

#ifdef _WIN32
#include <spdlog/sinks/wincolor_sink.h>
#else
#include <spdlog/sinks/ansicolor_sink.h>
#endif

#if defined(_DEBUG) && defined(_MSC_VER)
#include <spdlog/sinks/msvc_sink.h>
#endif  // _DEBUG && _MSC_VER

namespace header_only_lib {

	enum class LogLevel {
		info = spdlog::level::level_enum::info,
		trace = spdlog::level::level_enum::trace,
		debug = spdlog::level::level_enum::debug,
		error = spdlog::level::level_enum::err
	};

	class Logger {
	public:
		Logger(const std::string &section) {
			internal_logger_ = spdlog::get("console");

			if (internal_logger_ == nullptr) {
				internal_logger_ = create_spdlog();
			}
		}

		auto section() const noexcept { return section_; }

		template <typename Arg1, typename... Args>
		void log(const LogLevel level, const char *fmt, const Arg1 &arg1,
			const Args &... args) {
			auto new_fmt = "[{}] " + std::string(fmt);

			internal_logger_->log(static_cast<spdlog::level::level_enum>(level),
				new_fmt.c_str(), section(), arg1, args...);
		}

		template <typename Arg1, typename... Args>
		void info(const char *fmt, const Arg1 &arg1, const Args &... args) {
			log(LogLevel::info, fmt, arg1, args...);
		}

		template <typename Arg1>
		void info(const Arg1 &arg1) {
			info("{}", arg1);
		}

		template <typename Arg1, typename... Args>
		void debug(const char *fmt, const Arg1 &arg1, const Args &... args) {
			log(LogLevel::debug, fmt, arg1, args...);
		}

		template <typename Arg1>
		void debug(const Arg1 &arg1) {
			debug("{}", arg1);
		}

		template <typename Arg1, typename... Args>
		void trace(const char *fmt, const Arg1 &arg1, const Args &... args) {
			log(LogLevel::trace, fmt, arg1, args...);
		}

		template <typename Arg1>
		void trace(const Arg1 &arg1) {
			trace("{}", arg1);
		}

		template <typename Arg1, typename... Args>
		void error(const char *fmt, const Arg1 &arg1, const Args &... args) {
			log(LogLevel::error, fmt, arg1, args...);
		}

		template <typename Arg1>
		void error(const Arg1 &arg1) {
			error("{}", arg1);
		}

		static void level(const LogLevel level) noexcept {
			spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
		}

	private:
		std::string section_{ "" };
		std::shared_ptr<spdlog::logger> internal_logger_{ nullptr };
	};

	auto create_spdlog() {
#ifdef _WIN32
		auto color_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
		auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif

		auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_st>();
		dist_sink->add_sink(color_sink);

#if defined(_DEBUG) && defined(_MSC_VER)
		auto debug_sink = std::make_shared<spdlog::sinks::msvc_sink_st>();
		dist_sink->add_sink(debug_sink);
#endif  // _DEBUG && _MSC_VER

		return std::make_shared<spdlog::logger>("console", dist_sink);
	}

}  // namespace header_only_lib

#endif  // LOG_HPP
