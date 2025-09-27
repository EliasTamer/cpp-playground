#include <iostream>
#include <variant>
#include <string>
#include <cassert>
#include <vector>
#include <ranges>
#include <optional>
#include <charconv>


struct Info {
    auto operator<=>(const Info&) const = default;
};

struct Warning {
    auto operator<=>(const Warning&) const = default;
};

struct Error {
    int code;
    auto operator<=>(const Error&) const = default;
};

using MessageType = std::variant<Info, Warning, Error>;
using Timestamp = int;

struct Message{
    MessageType type;
    Timestamp timestamp;
    std::string msg;
    auto operator<=>(const Message&) const = default; 
};

struct Unknown {
    std::string msg;
    auto operator<=>(const Unknown&) const = default;
};

std::string unwords (const std::vector<std::string>& words, int to_drop){
    auto words_pipe = words 
    | std::views::drop(to_drop)
    | std::views::join_with(' ');
    
    return std::string(words_pipe.begin(), words_pipe.end());
}

using LogMessage = std::variant<Message, Unknown>;

LogMessage make_message(MessageType type, Timestamp timestamp, std::string msg) {
    return Message{type, timestamp, std::move(msg)};
};

std::optional<int> to_int(std::string_view sv) {
    int r;
    auto result = std::from_chars(sv.data(), sv.data() + sv.size(), r);

    if (result.ec == std::errc()){
        return r;
    }
    else {
        return std::nullopt;
    }
}

LogMessage parseMessage(std::string str){
    // Step 1:: Split words 
    auto words_pipe = str | std::views::split(' ')
    | std::views::transform([](const auto& range) {
        return std::string{range.begin(), range.end()};
    });

    //Step 2: Combine words
    std::vector<std::string> words;
    for (const std::string& word : words_pipe) {
        words.push_back(std::move(word));
    };

    auto get = [&words](size_t i) -> std::optional<std::string>{
        if (i < words.size()) {
            return words[i];
        }
        else {
            return std::nullopt;
        }
    };
    
    auto get_int = [&get](size_t i) -> std::optional<int> {
        return get(i).and_then(to_int);
    };

    return get(0).and_then([&get_int, &words](const std::string& msgTypeStr) {
        std::optional<LogMessage> result;

        if(msgTypeStr == "I") {
            result = 
                get_int(1).
                transform([&words](int timestamp) {
                    return make_message(Info{}, timestamp, unwords(words, 2));  
            });
        }
        else if(msgTypeStr == "W") {
            result = 
            get_int(1).
            transform([&words](int timestamp) {
                return make_message(Warning{}, timestamp, unwords(words, 2));
        });
        } 
        else if (msgTypeStr == "E"){
            result = 
            get_int(1).
            and_then([&words, &get_int](int code){
                    return get_int(2)
                    .transform([&words, code](int timestamp) {
                        return make_message(Error{code}, timestamp, unwords(words, 3));
                });
            });
        }
        else {
            result = std::nullopt;
        }
        return result;
    }).value_or(Unknown{str});
};

int main() {
    std::cout << "============== Program Starts Here ==============\n";
    
    assert(
        (parseMessage("I 29 help help") == 
        LogMessage{ Message {Info{}, 29, "help help"}}));
    
    assert(
    (parseMessage("E 2 562 help help") == 
    LogMessage{ Message {Error{2}, 562, "help help"}}));

    assert(
    (parseMessage("W 29 help help") == 
    LogMessage{ Message {Warning{}, 29, "help help"}}));

    assert(
    (parseMessage("Not in the right format!") == 
    LogMessage{ Unknown{"Not in the right format!"}}));
};