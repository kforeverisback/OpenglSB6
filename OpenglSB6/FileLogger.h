#ifndef FILELOGGER_HPP
#define FILELOGGER_HPP

#include <iostream>
#include <ctime>
#include <fstream>

// Use the namespace you want
namespace ige {

	class FileLogger : public ofstream{

	public:

		// If you can´t/dont-want-to use C++11, remove the "class" word after enum
		enum class e_logType { ERR, WARN, DBG };


		//explicit FileLogger(const char *engine_version, const char *fname = "OpenglSB6.log")
		// ctor (remove parameters if you don´t need them)
		explicit FileLogger(const char *fname = "OpenglSB6.log")
			: numWarnings(0U),
			numErrors(0U)
		{
			open(fname);

			// Write the first lines
			if (is_open()) {
				time_t now = time(NULL);
				char date[64] = { 0 };
				ctime_s(date, 64, &now);
				*this << "Created " << fname << " @local time " << date << std::endl;
				std::cout << "Created " << fname << " @local time " << date << std::endl;
			} // if

		}


		// dtor
		~FileLogger() {

			if (is_open()) {
				*this << std::endl << std::endl;

				// Report number of errors and warnings
				*this << numWarnings << " warnings" << std::endl;
				*this << numErrors << " errors" << std::endl;

				close();
			} // if

		}

		template<e_logType l_type>
		FileLogger& getLogger()
		{
			switch (l_type) {
			case ige::FileLogger::e_logType::ERR:
				*this << "[ERR]: ";
				++numErrors;
				break;

			case ige::FileLogger::e_logType::WARN:
				*this << "[WRN]: ";
				++numWarnings;
				break;

			default:
				*this << "[DBG]: ";
				break;
			} // sw
			return *this;
		}


		// Overload << operator using log type
		friend FileLogger &operator << (FileLogger &logger, const e_logType l_type) {

			switch (l_type) {
			case ige::FileLogger::e_logType::ERR:
				logger << "[ERR]: ";
				++logger.numErrors;
				break;

			case ige::FileLogger::e_logType::WARN:
				logger << "[WRN]: ";
				++logger.numWarnings;
				break;

			default:
				logger << "[DBG]: ";
				break;
			} // sw


			return logger;

		}

		// Make it Non Copyable (or you can inherit from sf::NonCopyable if you want)
		FileLogger(const FileLogger &) = delete;
		FileLogger &operator= (const FileLogger &) = delete;



	private:
		unsigned int            numWarnings;
		unsigned int            numErrors;

	}; // class end

}  // namespace


#endif // FILELOGGER_HPP