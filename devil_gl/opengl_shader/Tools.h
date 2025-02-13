﻿#pragma once
#include <memory>
#include <string>
#include <filesystem>
#include <iostream>
#include <sstream>

#include "Debug.h"

namespace Engine
{
	class Tools
	{
	public:
		template<typename ... Args>
		static std::string StringFormat(const std::string& format, Args ... args)
		{
			const int stringSize = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
			if (stringSize <= 0) throw "Error during formating";
			const auto size = static_cast<size_t>(stringSize);
			auto buffer = std::make_unique<char[]>(size);
			std::snprintf(buffer.get(), size, format.c_str(), args ...);
			// Return the stringBuffer without the \0
			return std::string(buffer.get(), buffer.get() + size - 1);
		}

		static std::vector<std::string> GetAssetFromDirectory(const std::string& directory)
		{
			std::vector<std::string> paths;
			if (!std::filesystem::is_directory(directory)) return paths;

			for (auto const& entry : std::filesystem::directory_iterator(directory))
			{
				std::string path = entry.path().generic_string();
				std::replace(path.begin(), path.end(), '\\', '/');
				paths.emplace_back(path);
			}
			return paths;
		}

		static std::string ChoiceListFromDirectory(const std::string& directory, const std::vector<std::string>& assets)
		{
			if (assets.empty())
			{
				Debug::LogError(Tools::StringFormat("No assets found in '%s'", directory.c_str()));
				return "";
			}

			uint32_t count = 0;
			std::stringstream ss;
			ss << "Choose between those assets : " << std::endl;
			for (auto& asset : assets)
			{
				ss << "\t- [" << count << "] " << asset << std::endl;
				count++;
			}

			ss << "Which one (-1 if you don't choose) ? ";

			return ss.str();
		}

		/* Processes an OpenGL error message callback */
		static void GLAPIENTRY GLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
			std::string warningMessage;
			switch (severity)
			{
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				warningMessage = "LOG";
				break;
			case GL_DEBUG_SEVERITY_LOW:
				warningMessage = "WARN";
				break;
			case GL_DEBUG_SEVERITY_MEDIUM:
				warningMessage = "ERROR";
				break;
			case GL_DEBUG_SEVERITY_HIGH:
				warningMessage = "FATAL";
				break;
			default:
				warningMessage = "UNKNOWN";
				break;
			}

			std::cout << "[OPENGL " << warningMessage << "](" << type << ") : " << message << std::endl;

			if (severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_HIGH)
			{
				std::cout << "Breaking execution because an error was raised above medium severity!" << std::endl;
				__debugbreak();
			}
		}
	};
}
