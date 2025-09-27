#include <algorithm>
#include <iostream>
#include <cassert>
#include <vector>
#include <ranges>
#include <utility>


std::vector<int> digits(long long n) {
    std::vector<int> result;

    while (n > 0) {
        int last_digit = n % 10;
        n /= 10; 
        result.push_back(last_digit);
    }
    return result;
}

bool validate1(long long card_number) {
    int sum = 0;
    bool is_even = false;

    while(card_number > 0){
        int digit = card_number % 10;
        card_number /= 10;

        if(is_even) {
            digit *= 2;
            int digit1 = digit % 10; 
            int digit2 = digit / 10;
            sum += digit1 + digit2;
        }else {
            sum += digit;
        }

        is_even = !is_even;
    }
    
    return sum % 10 == 0;
}


auto flatten_digits() {
    return std::views::transform([](int doubled_digit) {
    return digits(doubled_digit);
}) | std::views::join;
};

bool validate2(long long card_number) {
std::vector<int> card_digits = digits(card_number);

// { 1 2 3 4}
// { 1 4 3 8}
// {{1}, {4}, {3}, {8}}
auto doubled_digits = 
    std::views::zip(std::views::iota(1), card_digits) 
    | std::views::transform([](const auto& pair){
        auto [i, d] = pair;
        return i % 2 == 0 ? d * 2 : d;
    })
    | flatten_digits();

    int sum = std::ranges::fold_left(doubled_digits, 0, std::plus{});
    return sum % 10 == 0;
}


int main() {
    std::cout << "============== Program Starts Here ==============\n";
    assert(validate1(4012888888881881));
    assert(!validate1(4012888888881882));

    assert(validate2(4012888888881881));
    assert(!validate2(4012888888881882));

}