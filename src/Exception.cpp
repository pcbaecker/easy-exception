#include <ee/Exception.hpp>
#include <cstring>

namespace ee {

    Exception::Exception(
            std::string caller,
            std::string message,
            std::vector<ee::Note> infos,
            OutputFormat format) noexcept :
            mCaller(std::move(caller)),
            mMessage(std::move(message)),
            mInfos(std::move(infos)),
            mTimepoint(std::chrono::system_clock::now()),
            mFormat(format),
            mStacktrace(Stacktrace::create()) {
        this->update();
    }

    Exception &Exception::operator<<(const Note &info) noexcept {
        try {
            this->mInfos.emplace_back(info);
            this->update();
        } catch (...) {
            std::cerr << __PRETTY_FUNCTION__ << ": Could not store info" << std::endl;
        }
        return *this;
    }

    void Exception::update() noexcept {
        try {
            this->mCache.clear();
            std::string& str = this->mCache;

            // Format datetime to string
            char datetime[128];
            auto time = std::chrono::system_clock::to_time_t(this->mTimepoint);
            if (!std::strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", std::localtime(&time))) {
                memset(datetime, 0, sizeof(datetime));
            }

            // Determine the format
            switch (this->mFormat) {
                default:
                case String: {
                    // Format output as string
                    str += "Exception type:\n";
                    str += "\t" + std::string(typeid(this).name()) + "\n";
                    str += "Datetime:\n";
                    str += "\t" + std::string(datetime) + "\n";
                    if (!this->mCaller.empty()) {
                        str += "In method:\n";
                        str += "\t" + this->mCaller + "\n";
                    }
                    if (!this->mMessage.empty()) {
                        str += "With message:\n";
                        str += "\t" + this->mMessage + "\n";
                    }
                    if (!this->mInfos.empty()) {
                        for (const auto& info : this->mInfos) {
                            str += info.getName() + (!info.getCaller().empty() ? " { " + info.getCaller() + " }" : "") + ":\n";
                            str += "\t" + info.getValue() + "\n";
                        }
                    }
                    if (this->mStacktrace.has_value()
                        && !this->mStacktrace->get()->getLines().empty()) {
                        str += "Stacktrace:\n";
                        str += this->mStacktrace->get()->asString();
                    }
                } break;

                case Json: {
                    // Format output as json
                    str += "{\n";
                    str += "\"type\" : \"" + std::string(typeid(this).name()) + "\"";
                    str += ",\n\"datetime\" : \"" + std::string(datetime) + "\"";
                    if (!this->mCaller.empty()) {
                        str += ",\n\"method\" : \"" + this->mCaller + "\"";
                    }
                    if (!this->mMessage.empty()) {
                        str += ",\n\"message\" : \"" + this->mMessage + "\"";
                    }
                    if (!this->mInfos.empty()) {
                        str += ",\n\"infos\" : [\n";
                        uint16_t i = 0;
                        for (const auto& info : this->mInfos) {
                            str += "\t{\"name\" : \"" + info.getName() + "\",\n";
                            str += "\t\"value\" : \"" + info.getValue() + "\",\n";
                            str += "\t\"caller\" : " + (info.getCaller().empty() ? "null\n" : "\"" + info.getCaller() + "\"\n");
                            str += "\t}";
                            if (++i < this->mInfos.size()) {
                                str += ",";
                            }
                            str += "\n";
                        }
                        str += "]";
                    }
                    if (this->mStacktrace.has_value()
                        && !this->mStacktrace->get()->getLines().empty()) {
                        str += ",\n\"stacktrace\" : [\n";
                        uint16_t i = 0;
                        for (const auto& line : this->mStacktrace->get()->getLines()) {
                            str += "\t\"" + line.second + "\"";
                            if (++i < this->mStacktrace->get()->getLines().size()) {
                                str += ",";
                            }
                            str += "\n";
                        }
                        str += "]";
                    }
                    str += "\n}";
                }
            }
        } catch (...) {
            std::cerr << __PRETTY_FUNCTION__ << ": Could not build message" << std::endl;
            this->mCache = "Could not build message";
        }
    }

    const char *Exception::what() const noexcept {
        return this->mCache.c_str();
    }

    const std::string &Exception::getMessage() const noexcept {
        return this->mMessage;
    }

    const std::string &Exception::getCaller() const noexcept {
        return this->mCaller;
    }

    const std::vector<Note> &Exception::getNotes() const noexcept {
        return this->mInfos;
    }

    const std::optional<std::shared_ptr<Stacktrace>> &Exception::getStacktrace() const noexcept {
        return this->mStacktrace;
    }

    const std::chrono::time_point<std::chrono::system_clock> &Exception::getDateOfCreation() const noexcept {
        return this->mTimepoint;
    }

}