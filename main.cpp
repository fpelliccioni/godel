#include <cstddef>

#include <algorithm>
#include <array>

template <size_t N>
using chr_arr = std::array<char, N>;

template <size_t N>
struct ct_str {
    char data[N]{};

    constexpr
    ct_str() = default;

    constexpr
    ct_str(char const(&str)[N]) {
        std::copy_n(str, N, data);
        std::reverse(std::begin(data), std::end(data) - 1);
    };

    constexpr size_t size() const {
        return std::size(data);
    }

    friend constexpr
    bool operator==(ct_str const& x, ct_str const& y) {
        return std::equal(std::begin(x.data), std::end(x.data), std::begin(y.data));
    }
};

template <ct_str S>
constexpr auto operator""_n() {
    return std::to_array(S.data);
}

template <ct_str S>
constexpr auto repr() {
    auto data = std::to_array(S.data);
    // std::reverse(std::begin(data), std::end(data) - 1);
    return data;
}

// template <ct_str S>
// constexpr auto repr() {
//     char data[S.size()]{};
//     std::copy_n(S.data, S.size(), data);
//     std::reverse(std::begin(data), std::end(data) - 1);
//     return data;
// }

template <char X, size_t Y>
constexpr auto add_digit() {
    static_assert(X >= '0');
    static_assert(X <= '9');
    static_assert(Y >= '0');
    static_assert(Y <= '9');

    constexpr char r = X + Y - '0';

    if constexpr (r > '9') {
        return std::pair{true, char(r - 10)};
    }

    return std::pair{false, r};
}

template <auto Start, auto End, auto Inc, class F>
requires (Inc != 0)
constexpr
void constexpr_for(F&& f) {
    if constexpr (Inc > 0) {
        if constexpr (Start < End) {
            f(std::integral_constant<decltype(Start), Start>());
            constexpr_for<Start + Inc, End, Inc>(f);
        }
    } else {
        if constexpr (Start > End) {
            f(std::integral_constant<decltype(Start), Start + Inc>());
            constexpr_for<Start + Inc, End, Inc>(f);
        }
    }
}

template <ct_str X, ct_str Y>
constexpr bool will_overflow() {
    constexpr auto x = std::to_array(X.data);
    constexpr auto y = std::to_array(Y.data);
    constexpr auto SX = std::size(x);
    constexpr auto SY = std::size(y);
    constexpr auto M = std::max(SX, SY);

    bool overflow = false;

    constexpr_for<0, M - 1, 1>([&x, &y, &overflow](auto i) {
        constexpr char cx = (i < SX - 1)  ? x[i] : '0';
        constexpr char cy = (i < SY - 1)  ? y[i] : '0';

        constexpr auto r1 = add_digit<cx, cy>();

        if (r1.first) {
            overflow = true;
            return;
        }

        if (overflow) {
            constexpr auto r2 = add_digit<r1.second, '1'>();
            overflow = r2.first;
        }
    });

    return overflow;
}

template <ct_str X, ct_str Y>
constexpr auto add() {
    constexpr auto x = std::to_array(X.data);
    constexpr auto y = std::to_array(Y.data);
    constexpr auto SX = std::size(x);
    constexpr auto SY = std::size(y);
    constexpr auto O = int(will_overflow<X, Y>());
    constexpr auto Z = std::max(SX, SY) + O;

    // static_assert(SX == 2);
    // static_assert(SY == 2);
    // static_assert(will_overflow<X, Y>());
    // static_assert(O == 1);
    // static_assert(Z == 3);

    bool overflow = false;
    ct_str<Z> ret;

    constexpr_for<0, Z - O - 1, 1>([&x, &y, &ret, &overflow](auto i){

        constexpr char cx = (i < SX - 1)  ? x[i] : '0';
        constexpr char cy = (i < SY - 1)  ? y[i] : '0';

        constexpr auto r1 = add_digit<cx, cy>();

        if (overflow) {
            constexpr auto r2 = add_digit<r1.second, '1'>();
            ret.data[i] = r2.second;
            overflow = r2.first;
        } else {
            ret.data[i] = r1.second;
            overflow = r1.first;
        }
    });

    if (overflow) {
        ret.data[Z - 2] = '1';
    }
    return ret;
}

void will_overflow_static_tests() {
    static_assert( ! will_overflow<"0", "0">());
    static_assert( ! will_overflow<"0", "1">());
    static_assert( ! will_overflow<"0", "2">());
    static_assert( ! will_overflow<"0", "3">());
    static_assert( ! will_overflow<"0", "4">());
    static_assert( ! will_overflow<"0", "5">());
    static_assert( ! will_overflow<"0", "6">());
    static_assert( ! will_overflow<"0", "7">());
    static_assert( ! will_overflow<"0", "8">());
    static_assert( ! will_overflow<"0", "9">());

    static_assert( ! will_overflow<"1", "0">());
    static_assert( ! will_overflow<"1", "1">());
    static_assert( ! will_overflow<"1", "2">());
    static_assert( ! will_overflow<"1", "3">());
    static_assert( ! will_overflow<"1", "4">());
    static_assert( ! will_overflow<"1", "5">());
    static_assert( ! will_overflow<"1", "6">());
    static_assert( ! will_overflow<"1", "7">());
    static_assert( ! will_overflow<"1", "8">());
    static_assert(   will_overflow<"1", "9">());

    static_assert( ! will_overflow<"2", "0">());
    static_assert( ! will_overflow<"2", "1">());
    static_assert( ! will_overflow<"2", "2">());
    static_assert( ! will_overflow<"2", "3">());
    static_assert( ! will_overflow<"2", "4">());
    static_assert( ! will_overflow<"2", "5">());
    static_assert( ! will_overflow<"2", "6">());
    static_assert( ! will_overflow<"2", "7">());
    static_assert(   will_overflow<"2", "8">());
    static_assert(   will_overflow<"2", "9">());

    static_assert( ! will_overflow<"3", "0">());
    static_assert( ! will_overflow<"3", "1">());
    static_assert( ! will_overflow<"3", "2">());
    static_assert( ! will_overflow<"3", "3">());
    static_assert( ! will_overflow<"3", "4">());
    static_assert( ! will_overflow<"3", "5">());
    static_assert( ! will_overflow<"3", "6">());
    static_assert(   will_overflow<"3", "7">());
    static_assert(   will_overflow<"3", "8">());
    static_assert(   will_overflow<"3", "9">());

    static_assert( ! will_overflow<"4", "0">());
    static_assert( ! will_overflow<"4", "1">());
    static_assert( ! will_overflow<"4", "2">());
    static_assert( ! will_overflow<"4", "3">());
    static_assert( ! will_overflow<"4", "4">());
    static_assert( ! will_overflow<"4", "5">());
    static_assert(   will_overflow<"4", "6">());
    static_assert(   will_overflow<"4", "7">());
    static_assert(   will_overflow<"4", "8">());
    static_assert(   will_overflow<"4", "9">());

    static_assert( ! will_overflow<"5", "0">());
    static_assert( ! will_overflow<"5", "1">());
    static_assert( ! will_overflow<"5", "2">());
    static_assert( ! will_overflow<"5", "3">());
    static_assert( ! will_overflow<"5", "4">());
    static_assert(   will_overflow<"5", "5">());
    static_assert(   will_overflow<"5", "6">());
    static_assert(   will_overflow<"5", "7">());
    static_assert(   will_overflow<"5", "8">());
    static_assert(   will_overflow<"5", "9">());

    static_assert( ! will_overflow<"6", "0">());
    static_assert( ! will_overflow<"6", "1">());
    static_assert( ! will_overflow<"6", "2">());
    static_assert( ! will_overflow<"6", "3">());
    static_assert(   will_overflow<"6", "4">());
    static_assert(   will_overflow<"6", "5">());
    static_assert(   will_overflow<"6", "6">());
    static_assert(   will_overflow<"6", "7">());
    static_assert(   will_overflow<"6", "8">());
    static_assert(   will_overflow<"6", "9">());

    static_assert( ! will_overflow<"7", "0">());
    static_assert( ! will_overflow<"7", "1">());
    static_assert( ! will_overflow<"7", "2">());
    static_assert(   will_overflow<"7", "3">());
    static_assert(   will_overflow<"7", "4">());
    static_assert(   will_overflow<"7", "5">());
    static_assert(   will_overflow<"7", "6">());
    static_assert(   will_overflow<"7", "7">());
    static_assert(   will_overflow<"7", "8">());
    static_assert(   will_overflow<"7", "9">());

    static_assert( ! will_overflow<"8", "0">());
    static_assert( ! will_overflow<"8", "1">());
    static_assert(   will_overflow<"8", "2">());
    static_assert(   will_overflow<"8", "3">());
    static_assert(   will_overflow<"8", "4">());
    static_assert(   will_overflow<"8", "5">());
    static_assert(   will_overflow<"8", "6">());
    static_assert(   will_overflow<"8", "7">());
    static_assert(   will_overflow<"8", "8">());
    static_assert(   will_overflow<"8", "9">());

    static_assert( ! will_overflow<"9", "0">());
    static_assert(   will_overflow<"9", "1">());
    static_assert(   will_overflow<"9", "2">());
    static_assert(   will_overflow<"9", "3">());
    static_assert(   will_overflow<"9", "4">());
    static_assert(   will_overflow<"9", "5">());
    static_assert(   will_overflow<"9", "6">());
    static_assert(   will_overflow<"9", "7">());
    static_assert(   will_overflow<"9", "8">());
    static_assert(   will_overflow<"9", "9">());

    static_assert( ! will_overflow<"10", "0">());
    static_assert( ! will_overflow<"10", "1">());
    static_assert( ! will_overflow<"10", "2">());
    static_assert( ! will_overflow<"10", "3">());
    static_assert( ! will_overflow<"10", "4">());
    static_assert( ! will_overflow<"10", "5">());
    static_assert( ! will_overflow<"10", "6">());
    static_assert( ! will_overflow<"10", "7">());
    static_assert( ! will_overflow<"10", "8">());
    static_assert( ! will_overflow<"10", "9">());

    static_assert( ! will_overflow<"11", "0">());
    static_assert( ! will_overflow<"11", "1">());
    static_assert( ! will_overflow<"11", "2">());
    static_assert( ! will_overflow<"11", "3">());
    static_assert( ! will_overflow<"11", "4">());
    static_assert( ! will_overflow<"11", "5">());
    static_assert( ! will_overflow<"11", "6">());
    static_assert( ! will_overflow<"11", "7">());
    static_assert( ! will_overflow<"11", "8">());
    static_assert( ! will_overflow<"11", "9">());

    static_assert( ! will_overflow<"12", "0">());
    static_assert( ! will_overflow<"12", "1">());
    static_assert( ! will_overflow<"12", "2">());
    static_assert( ! will_overflow<"12", "3">());
    static_assert( ! will_overflow<"12", "4">());
    static_assert( ! will_overflow<"12", "5">());
    static_assert( ! will_overflow<"12", "6">());
    static_assert( ! will_overflow<"12", "7">());
    static_assert( ! will_overflow<"12", "8">());
    static_assert( ! will_overflow<"12", "9">());

    static_assert( ! will_overflow<"13", "0">());
    static_assert( ! will_overflow<"13", "1">());
    static_assert( ! will_overflow<"13", "2">());
    static_assert( ! will_overflow<"13", "3">());
    static_assert( ! will_overflow<"13", "4">());
    static_assert( ! will_overflow<"13", "5">());
    static_assert( ! will_overflow<"13", "6">());
    static_assert( ! will_overflow<"13", "7">());
    static_assert( ! will_overflow<"13", "8">());
    static_assert( ! will_overflow<"13", "9">());

    static_assert( ! will_overflow<"14", "0">());
    static_assert( ! will_overflow<"14", "1">());
    static_assert( ! will_overflow<"14", "2">());
    static_assert( ! will_overflow<"14", "3">());
    static_assert( ! will_overflow<"14", "4">());
    static_assert( ! will_overflow<"14", "5">());
    static_assert( ! will_overflow<"14", "6">());
    static_assert( ! will_overflow<"14", "7">());
    static_assert( ! will_overflow<"14", "8">());
    static_assert( ! will_overflow<"14", "9">());

    static_assert( ! will_overflow<"15", "0">());
    static_assert( ! will_overflow<"15", "1">());
    static_assert( ! will_overflow<"15", "2">());
    static_assert( ! will_overflow<"15", "3">());
    static_assert( ! will_overflow<"15", "4">());
    static_assert( ! will_overflow<"15", "5">());
    static_assert( ! will_overflow<"15", "6">());
    static_assert( ! will_overflow<"15", "7">());
    static_assert( ! will_overflow<"15", "8">());
    static_assert( ! will_overflow<"15", "9">());

    static_assert( ! will_overflow<"16", "0">());
    static_assert( ! will_overflow<"16", "1">());
    static_assert( ! will_overflow<"16", "2">());
    static_assert( ! will_overflow<"16", "3">());
    static_assert( ! will_overflow<"16", "4">());
    static_assert( ! will_overflow<"16", "5">());
    static_assert( ! will_overflow<"16", "6">());
    static_assert( ! will_overflow<"16", "7">());
    static_assert( ! will_overflow<"16", "8">());
    static_assert( ! will_overflow<"16", "9">());

    static_assert( ! will_overflow<"17", "0">());
    static_assert( ! will_overflow<"17", "1">());
    static_assert( ! will_overflow<"17", "2">());
    static_assert( ! will_overflow<"17", "3">());
    static_assert( ! will_overflow<"17", "4">());
    static_assert( ! will_overflow<"17", "5">());
    static_assert( ! will_overflow<"17", "6">());
    static_assert( ! will_overflow<"17", "7">());
    static_assert( ! will_overflow<"17", "8">());
    static_assert( ! will_overflow<"17", "9">());

    static_assert( ! will_overflow<"18", "0">());
    static_assert( ! will_overflow<"18", "1">());
    static_assert( ! will_overflow<"18", "2">());
    static_assert( ! will_overflow<"18", "3">());
    static_assert( ! will_overflow<"18", "4">());
    static_assert( ! will_overflow<"18", "5">());
    static_assert( ! will_overflow<"18", "6">());
    static_assert( ! will_overflow<"18", "7">());
    static_assert( ! will_overflow<"18", "8">());
    static_assert( ! will_overflow<"18", "9">());

    static_assert( ! will_overflow<"19", "0">());
    static_assert( ! will_overflow<"19", "1">());
    static_assert( ! will_overflow<"19", "2">());
    static_assert( ! will_overflow<"19", "3">());
    static_assert( ! will_overflow<"19", "4">());
    static_assert( ! will_overflow<"19", "5">());
    static_assert( ! will_overflow<"19", "6">());
    static_assert( ! will_overflow<"19", "7">());
    static_assert( ! will_overflow<"19", "8">());
    static_assert( ! will_overflow<"19", "9">());

    static_assert( ! will_overflow<"29", "1">());
    static_assert( ! will_overflow<"39", "1">());
    static_assert( ! will_overflow<"49", "1">());
    static_assert( ! will_overflow<"59", "1">());
    static_assert( ! will_overflow<"69", "1">());
    static_assert( ! will_overflow<"79", "1">());
    static_assert( ! will_overflow<"89", "1">());
    static_assert(   will_overflow<"99", "1">());

    static_assert(   will_overflow<"29", "71">());
    static_assert(   will_overflow<"39", "61">());
    static_assert(   will_overflow<"49", "51">());
    static_assert(   will_overflow<"59", "41">());
    static_assert(   will_overflow<"69", "31">());
    static_assert(   will_overflow<"79", "21">());
    static_assert(   will_overflow<"89", "11">());

    static_assert( ! will_overflow<"29", "70">());
    static_assert( ! will_overflow<"39", "60">());
    static_assert( ! will_overflow<"49", "50">());
    static_assert( ! will_overflow<"59", "40">());
    static_assert( ! will_overflow<"69", "30">());
    static_assert( ! will_overflow<"79", "20">());
    static_assert( ! will_overflow<"89", "10">());
}

void add_digit_static_tests() {
    static_assert(add_digit<'0', '0'>() == std::pair{false, '0'});
    static_assert(add_digit<'0', '1'>() == std::pair{false, '1'});
    static_assert(add_digit<'0', '2'>() == std::pair{false, '2'});
    static_assert(add_digit<'0', '3'>() == std::pair{false, '3'});
    static_assert(add_digit<'0', '4'>() == std::pair{false, '4'});
    static_assert(add_digit<'0', '5'>() == std::pair{false, '5'});
    static_assert(add_digit<'0', '6'>() == std::pair{false, '6'});
    static_assert(add_digit<'0', '7'>() == std::pair{false, '7'});
    static_assert(add_digit<'0', '8'>() == std::pair{false, '8'});
    static_assert(add_digit<'0', '9'>() == std::pair{false, '9'});

    static_assert(add_digit<'1', '0'>() == std::pair{false, '1'});
    static_assert(add_digit<'1', '1'>() == std::pair{false, '2'});
    static_assert(add_digit<'1', '2'>() == std::pair{false, '3'});
    static_assert(add_digit<'1', '3'>() == std::pair{false, '4'});
    static_assert(add_digit<'1', '4'>() == std::pair{false, '5'});
    static_assert(add_digit<'1', '5'>() == std::pair{false, '6'});
    static_assert(add_digit<'1', '6'>() == std::pair{false, '7'});
    static_assert(add_digit<'1', '7'>() == std::pair{false, '8'});
    static_assert(add_digit<'1', '8'>() == std::pair{false, '9'});
    static_assert(add_digit<'1', '9'>() == std::pair{true, '0'});


    static_assert(add_digit<'2', '0'>() == std::pair{false, '2'});
    static_assert(add_digit<'2', '1'>() == std::pair{false, '3'});
    static_assert(add_digit<'2', '2'>() == std::pair{false, '4'});
    static_assert(add_digit<'2', '3'>() == std::pair{false, '5'});
    static_assert(add_digit<'2', '4'>() == std::pair{false, '6'});
    static_assert(add_digit<'2', '5'>() == std::pair{false, '7'});
    static_assert(add_digit<'2', '6'>() == std::pair{false, '8'});
    static_assert(add_digit<'2', '7'>() == std::pair{false, '9'});
    static_assert(add_digit<'2', '8'>() == std::pair{true, '0'});
    static_assert(add_digit<'2', '9'>() == std::pair{true, '1'});

    static_assert(add_digit<'3', '0'>() == std::pair{false, '3'});
    static_assert(add_digit<'3', '1'>() == std::pair{false, '4'});
    static_assert(add_digit<'3', '2'>() == std::pair{false, '5'});
    static_assert(add_digit<'3', '3'>() == std::pair{false, '6'});
    static_assert(add_digit<'3', '4'>() == std::pair{false, '7'});
    static_assert(add_digit<'3', '5'>() == std::pair{false, '8'});
    static_assert(add_digit<'3', '6'>() == std::pair{false, '9'});
    static_assert(add_digit<'3', '7'>() == std::pair{true, '0'});
    static_assert(add_digit<'3', '8'>() == std::pair{true, '1'});
    static_assert(add_digit<'3', '9'>() == std::pair{true, '2'});

    static_assert(add_digit<'4', '0'>() == std::pair{false, '4'});
    static_assert(add_digit<'4', '1'>() == std::pair{false, '5'});
    static_assert(add_digit<'4', '2'>() == std::pair{false, '6'});
    static_assert(add_digit<'4', '3'>() == std::pair{false, '7'});
    static_assert(add_digit<'4', '4'>() == std::pair{false, '8'});
    static_assert(add_digit<'4', '5'>() == std::pair{false, '9'});
    static_assert(add_digit<'4', '6'>() == std::pair{true, '0'});
    static_assert(add_digit<'4', '7'>() == std::pair{true, '1'});
    static_assert(add_digit<'4', '8'>() == std::pair{true, '2'});
    static_assert(add_digit<'4', '9'>() == std::pair{true, '3'});

    static_assert(add_digit<'5', '0'>() == std::pair{false, '5'});
    static_assert(add_digit<'5', '1'>() == std::pair{false, '6'});
    static_assert(add_digit<'5', '2'>() == std::pair{false, '7'});
    static_assert(add_digit<'5', '3'>() == std::pair{false, '8'});
    static_assert(add_digit<'5', '4'>() == std::pair{false, '9'});
    static_assert(add_digit<'5', '5'>() == std::pair{true, '0'});
    static_assert(add_digit<'5', '6'>() == std::pair{true, '1'});
    static_assert(add_digit<'5', '7'>() == std::pair{true, '2'});
    static_assert(add_digit<'5', '8'>() == std::pair{true, '3'});
    static_assert(add_digit<'5', '9'>() == std::pair{true, '4'});

    static_assert(add_digit<'6', '0'>() == std::pair{false, '6'});
    static_assert(add_digit<'6', '1'>() == std::pair{false, '7'});
    static_assert(add_digit<'6', '2'>() == std::pair{false, '8'});
    static_assert(add_digit<'6', '3'>() == std::pair{false, '9'});
    static_assert(add_digit<'6', '4'>() == std::pair{true, '0'});
    static_assert(add_digit<'6', '5'>() == std::pair{true, '1'});
    static_assert(add_digit<'6', '6'>() == std::pair{true, '2'});
    static_assert(add_digit<'6', '7'>() == std::pair{true, '3'});
    static_assert(add_digit<'6', '8'>() == std::pair{true, '4'});
    static_assert(add_digit<'6', '9'>() == std::pair{true, '5'});

    static_assert(add_digit<'7', '0'>() == std::pair{false, '7'});
    static_assert(add_digit<'7', '1'>() == std::pair{false, '8'});
    static_assert(add_digit<'7', '2'>() == std::pair{false, '9'});
    static_assert(add_digit<'7', '3'>() == std::pair{true, '0'});
    static_assert(add_digit<'7', '4'>() == std::pair{true, '1'});
    static_assert(add_digit<'7', '5'>() == std::pair{true, '2'});
    static_assert(add_digit<'7', '6'>() == std::pair{true, '3'});
    static_assert(add_digit<'7', '7'>() == std::pair{true, '4'});
    static_assert(add_digit<'7', '8'>() == std::pair{true, '5'});
    static_assert(add_digit<'7', '9'>() == std::pair{true, '6'});

    static_assert(add_digit<'8', '0'>() == std::pair{false, '8'});
    static_assert(add_digit<'8', '1'>() == std::pair{false, '9'});
    static_assert(add_digit<'8', '2'>() == std::pair{true, '0'});
    static_assert(add_digit<'8', '3'>() == std::pair{true, '1'});
    static_assert(add_digit<'8', '4'>() == std::pair{true, '2'});
    static_assert(add_digit<'8', '5'>() == std::pair{true, '3'});
    static_assert(add_digit<'8', '6'>() == std::pair{true, '4'});
    static_assert(add_digit<'8', '7'>() == std::pair{true, '5'});
    static_assert(add_digit<'8', '8'>() == std::pair{true, '6'});
    static_assert(add_digit<'8', '9'>() == std::pair{true, '7'});

    static_assert(add_digit<'9', '0'>() == std::pair{false, '9'});
    static_assert(add_digit<'9', '1'>() == std::pair{true, '0'});
    static_assert(add_digit<'9', '2'>() == std::pair{true, '1'});
    static_assert(add_digit<'9', '3'>() == std::pair{true, '2'});
    static_assert(add_digit<'9', '4'>() == std::pair{true, '3'});
    static_assert(add_digit<'9', '5'>() == std::pair{true, '4'});
    static_assert(add_digit<'9', '6'>() == std::pair{true, '5'});
    static_assert(add_digit<'9', '7'>() == std::pair{true, '6'});
    static_assert(add_digit<'9', '8'>() == std::pair{true, '7'});
    static_assert(add_digit<'9', '9'>() == std::pair{true, '8'});
}

void add_static_tests() {
    static_assert(add<"0", "0">() == ct_str("0"));
    static_assert(add<"0", "1">() == ct_str("1"));
    static_assert(add<"0", "2">() == ct_str("2"));
    static_assert(add<"0", "3">() == ct_str("3"));
    static_assert(add<"0", "4">() == ct_str("4"));
    static_assert(add<"0", "5">() == ct_str("5"));
    static_assert(add<"0", "6">() == ct_str("6"));
    static_assert(add<"0", "7">() == ct_str("7"));
    static_assert(add<"0", "8">() == ct_str("8"));
    static_assert(add<"0", "9">() == ct_str("9"));

    static_assert(add<"1", "0">() == ct_str("1"));
    static_assert(add<"1", "1">() == ct_str("2"));
    static_assert(add<"1", "2">() == ct_str("3"));
    static_assert(add<"1", "3">() == ct_str("4"));
    static_assert(add<"1", "4">() == ct_str("5"));
    static_assert(add<"1", "5">() == ct_str("6"));
    static_assert(add<"1", "6">() == ct_str("7"));
    static_assert(add<"1", "7">() == ct_str("8"));
    static_assert(add<"1", "8">() == ct_str("9"));
    static_assert(add<"1", "9">() == ct_str("10"));

    static_assert(add<"2", "0">() == ct_str("2"));
    static_assert(add<"2", "1">() == ct_str("3"));
    static_assert(add<"2", "2">() == ct_str("4"));
    static_assert(add<"2", "3">() == ct_str("5"));
    static_assert(add<"2", "4">() == ct_str("6"));
    static_assert(add<"2", "5">() == ct_str("7"));
    static_assert(add<"2", "6">() == ct_str("8"));
    static_assert(add<"2", "7">() == ct_str("9"));
    static_assert(add<"2", "8">() == ct_str("10"));
    static_assert(add<"2", "9">() == ct_str("11"));

    static_assert(add<"3", "0">() == ct_str("3"));
    static_assert(add<"3", "1">() == ct_str("4"));
    static_assert(add<"3", "2">() == ct_str("5"));
    static_assert(add<"3", "3">() == ct_str("6"));
    static_assert(add<"3", "4">() == ct_str("7"));
    static_assert(add<"3", "5">() == ct_str("8"));
    static_assert(add<"3", "6">() == ct_str("9"));
    static_assert(add<"3", "7">() == ct_str("10"));
    static_assert(add<"3", "8">() == ct_str("11"));
    static_assert(add<"3", "9">() == ct_str("12"));

    static_assert(add<"4", "0">() == ct_str("4"));
    static_assert(add<"4", "1">() == ct_str("5"));
    static_assert(add<"4", "2">() == ct_str("6"));
    static_assert(add<"4", "3">() == ct_str("7"));
    static_assert(add<"4", "4">() == ct_str("8"));
    static_assert(add<"4", "5">() == ct_str("9"));
    static_assert(add<"4", "6">() == ct_str("10"));
    static_assert(add<"4", "7">() == ct_str("11"));
    static_assert(add<"4", "8">() == ct_str("12"));
    static_assert(add<"4", "9">() == ct_str("13"));

    static_assert(add<"5", "0">() == ct_str("5"));
    static_assert(add<"5", "1">() == ct_str("6"));
    static_assert(add<"5", "2">() == ct_str("7"));
    static_assert(add<"5", "3">() == ct_str("8"));
    static_assert(add<"5", "4">() == ct_str("9"));
    static_assert(add<"5", "5">() == ct_str("10"));
    static_assert(add<"5", "6">() == ct_str("11"));
    static_assert(add<"5", "7">() == ct_str("12"));
    static_assert(add<"5", "8">() == ct_str("13"));
    static_assert(add<"5", "9">() == ct_str("14"));

    static_assert(add<"6", "0">() == ct_str("6"));
    static_assert(add<"6", "1">() == ct_str("7"));
    static_assert(add<"6", "2">() == ct_str("8"));
    static_assert(add<"6", "3">() == ct_str("9"));
    static_assert(add<"6", "4">() == ct_str("10"));
    static_assert(add<"6", "5">() == ct_str("11"));
    static_assert(add<"6", "6">() == ct_str("12"));
    static_assert(add<"6", "7">() == ct_str("13"));
    static_assert(add<"6", "8">() == ct_str("14"));
    static_assert(add<"6", "9">() == ct_str("15"));

    static_assert(add<"7", "0">() == ct_str("7"));
    static_assert(add<"7", "1">() == ct_str("8"));
    static_assert(add<"7", "2">() == ct_str("9"));
    static_assert(add<"7", "3">() == ct_str("10"));
    static_assert(add<"7", "4">() == ct_str("11"));
    static_assert(add<"7", "5">() == ct_str("12"));
    static_assert(add<"7", "6">() == ct_str("13"));
    static_assert(add<"7", "7">() == ct_str("14"));
    static_assert(add<"7", "8">() == ct_str("15"));
    static_assert(add<"7", "9">() == ct_str("16"));

    static_assert(add<"8", "0">() == ct_str("8"));
    static_assert(add<"8", "1">() == ct_str("9"));
    static_assert(add<"8", "2">() == ct_str("10"));
    static_assert(add<"8", "3">() == ct_str("11"));
    static_assert(add<"8", "4">() == ct_str("12"));
    static_assert(add<"8", "5">() == ct_str("13"));
    static_assert(add<"8", "6">() == ct_str("14"));
    static_assert(add<"8", "7">() == ct_str("15"));
    static_assert(add<"8", "8">() == ct_str("16"));
    static_assert(add<"8", "9">() == ct_str("17"));

    static_assert(add<"9", "0">() == ct_str("9"));
    static_assert(add<"9", "1">() == ct_str("10"));
    static_assert(add<"9", "2">() == ct_str("11"));
    static_assert(add<"9", "3">() == ct_str("12"));
    static_assert(add<"9", "4">() == ct_str("13"));
    static_assert(add<"9", "5">() == ct_str("14"));
    static_assert(add<"9", "6">() == ct_str("15"));
    static_assert(add<"9", "7">() == ct_str("16"));
    static_assert(add<"9", "8">() == ct_str("17"));
    static_assert(add<"9", "9">() == ct_str("18"));

    static_assert(add<"10", "0">() == ct_str("10"));
    static_assert(add<"10", "1">() == ct_str("11"));
    static_assert(add<"10", "2">() == ct_str("12"));
    static_assert(add<"10", "3">() == ct_str("13"));
    static_assert(add<"10", "4">() == ct_str("14"));
    static_assert(add<"10", "5">() == ct_str("15"));
    static_assert(add<"10", "6">() == ct_str("16"));
    static_assert(add<"10", "7">() == ct_str("17"));
    static_assert(add<"10", "8">() == ct_str("18"));
    static_assert(add<"10", "9">() == ct_str("19"));

    static_assert(add<"11", "0">() == ct_str("11"));
    static_assert(add<"11", "1">() == ct_str("12"));
    static_assert(add<"11", "2">() == ct_str("13"));
    static_assert(add<"11", "3">() == ct_str("14"));
    static_assert(add<"11", "4">() == ct_str("15"));
    static_assert(add<"11", "5">() == ct_str("16"));
    static_assert(add<"11", "6">() == ct_str("17"));
    static_assert(add<"11", "7">() == ct_str("18"));
    static_assert(add<"11", "8">() == ct_str("19"));
    static_assert(add<"11", "9">() == ct_str("20"));

    static_assert(add<"12", "0">() == ct_str("12"));
    static_assert(add<"12", "1">() == ct_str("13"));
    static_assert(add<"12", "2">() == ct_str("14"));
    static_assert(add<"12", "3">() == ct_str("15"));
    static_assert(add<"12", "4">() == ct_str("16"));
    static_assert(add<"12", "5">() == ct_str("17"));
    static_assert(add<"12", "6">() == ct_str("18"));
    static_assert(add<"12", "7">() == ct_str("19"));
    static_assert(add<"12", "8">() == ct_str("20"));
    static_assert(add<"12", "9">() == ct_str("21"));

    static_assert(add<"13", "0">() == ct_str("13"));
    static_assert(add<"13", "1">() == ct_str("14"));
    static_assert(add<"13", "2">() == ct_str("15"));
    static_assert(add<"13", "3">() == ct_str("16"));
    static_assert(add<"13", "4">() == ct_str("17"));
    static_assert(add<"13", "5">() == ct_str("18"));
    static_assert(add<"13", "6">() == ct_str("19"));
    static_assert(add<"13", "7">() == ct_str("20"));
    static_assert(add<"13", "8">() == ct_str("21"));
    static_assert(add<"13", "9">() == ct_str("22"));

    static_assert(add<"14", "0">() == ct_str("14"));
    static_assert(add<"14", "1">() == ct_str("15"));
    static_assert(add<"14", "2">() == ct_str("16"));
    static_assert(add<"14", "3">() == ct_str("17"));
    static_assert(add<"14", "4">() == ct_str("18"));
    static_assert(add<"14", "5">() == ct_str("19"));
    static_assert(add<"14", "6">() == ct_str("20"));
    static_assert(add<"14", "7">() == ct_str("21"));
    static_assert(add<"14", "8">() == ct_str("22"));
    static_assert(add<"14", "9">() == ct_str("23"));

    static_assert(add<"15", "0">() == ct_str("15"));
    static_assert(add<"15", "1">() == ct_str("16"));
    static_assert(add<"15", "2">() == ct_str("17"));
    static_assert(add<"15", "3">() == ct_str("18"));
    static_assert(add<"15", "4">() == ct_str("19"));
    static_assert(add<"15", "5">() == ct_str("20"));
    static_assert(add<"15", "6">() == ct_str("21"));
    static_assert(add<"15", "7">() == ct_str("22"));
    static_assert(add<"15", "8">() == ct_str("23"));
    static_assert(add<"15", "9">() == ct_str("24"));

    static_assert(add<"16", "0">() == ct_str("16"));
    static_assert(add<"16", "1">() == ct_str("17"));
    static_assert(add<"16", "2">() == ct_str("18"));
    static_assert(add<"16", "3">() == ct_str("19"));
    static_assert(add<"16", "4">() == ct_str("20"));
    static_assert(add<"16", "5">() == ct_str("21"));
    static_assert(add<"16", "6">() == ct_str("22"));
    static_assert(add<"16", "7">() == ct_str("23"));
    static_assert(add<"16", "8">() == ct_str("24"));
    static_assert(add<"16", "9">() == ct_str("25"));

    static_assert(add<"17", "0">() == ct_str("17"));
    static_assert(add<"17", "1">() == ct_str("18"));
    static_assert(add<"17", "2">() == ct_str("19"));
    static_assert(add<"17", "3">() == ct_str("20"));
    static_assert(add<"17", "4">() == ct_str("21"));
    static_assert(add<"17", "5">() == ct_str("22"));
    static_assert(add<"17", "6">() == ct_str("23"));
    static_assert(add<"17", "7">() == ct_str("24"));
    static_assert(add<"17", "8">() == ct_str("25"));
    static_assert(add<"17", "9">() == ct_str("26"));

    static_assert(add<"18", "0">() == ct_str("18"));
    static_assert(add<"18", "1">() == ct_str("19"));
    static_assert(add<"18", "2">() == ct_str("20"));
    static_assert(add<"18", "3">() == ct_str("21"));
    static_assert(add<"18", "4">() == ct_str("22"));
    static_assert(add<"18", "5">() == ct_str("23"));
    static_assert(add<"18", "6">() == ct_str("24"));
    static_assert(add<"18", "7">() == ct_str("25"));
    static_assert(add<"18", "8">() == ct_str("26"));
    static_assert(add<"18", "9">() == ct_str("27"));

    static_assert(add<"19", "0">() == ct_str("19"));
    static_assert(add<"19", "1">() == ct_str("20"));
    static_assert(add<"19", "2">() == ct_str("21"));
    static_assert(add<"19", "3">() == ct_str("22"));
    static_assert(add<"19", "4">() == ct_str("23"));
    static_assert(add<"19", "5">() == ct_str("24"));
    static_assert(add<"19", "6">() == ct_str("25"));
    static_assert(add<"19", "7">() == ct_str("26"));
    static_assert(add<"19", "8">() == ct_str("27"));
    static_assert(add<"19", "9">() == ct_str("28"));

    static_assert(add<"29", "1">() == ct_str("30"));
    static_assert(add<"39", "1">() == ct_str("40"));
    static_assert(add<"49", "1">() == ct_str("50"));
    static_assert(add<"59", "1">() == ct_str("60"));
    static_assert(add<"69", "1">() == ct_str("70"));
    static_assert(add<"79", "1">() == ct_str("80"));
    static_assert(add<"89", "1">() == ct_str("90"));
    static_assert(add<"99", "1">() == ct_str("100"));

    static_assert(add<"29", "71">() == ct_str("100"));
    static_assert(add<"39", "61">() == ct_str("100"));
    static_assert(add<"49", "51">() == ct_str("100"));
    static_assert(add<"59", "41">() == ct_str("100"));
    static_assert(add<"69", "31">() == ct_str("100"));
    static_assert(add<"79", "21">() == ct_str("100"));
    static_assert(add<"89", "11">() == ct_str("100"));

    static_assert(add<"29", "70">() == ct_str("99"));
    static_assert(add<"39", "60">() == ct_str("99"));
    static_assert(add<"49", "50">() == ct_str("99"));
    static_assert(add<"59", "40">() == ct_str("99"));
    static_assert(add<"69", "30">() == ct_str("99"));
    static_assert(add<"79", "20">() == ct_str("99"));
    static_assert(add<"89", "10">() == ct_str("99"));

    static_assert(add<"99", "1">() == ct_str("100"));
    static_assert(add<"999", "1">() == ct_str("1000"));
    static_assert(add<"9999", "1">() == ct_str("10000"));
    static_assert(add<"99999", "1">() == ct_str("100000"));
    static_assert(add<"999999", "1">() == ct_str("1000000"));
    static_assert(add<"9999999", "1">() == ct_str("10000000"));
    static_assert(add<"99999999", "1">() == ct_str("100000000"));
    static_assert(add<"999999999", "1">() == ct_str("1000000000"));
}

#include <iostream>

int main() {
    // static_assert("123"_n == chr_arr<4>{'1','2','3','\0'});

    // static_assert(std::to_array(add<"1", "2">().data) == chr_arr<2>{'3', '\0'});
    // static_assert(std::to_array(add<"11", "22">().data) == chr_arr<3>{'3','3','\0'});
    // static_assert(std::to_array(add<"9", "9">().data) == chr_arr<3>{'1','8','\0'});
    // static_assert(std::to_array(add<"99", "9">().data) == chr_arr<3>{'1','8','\0'});

    // {
    //     auto xxx = repr<add<"1", "9">()>();
    //     std::cout << "size: " << xxx.size() << std::endl;

    //     for (auto x : xxx) {
    //         std::cout << "char: " << x << std::endl;
    //     }
    // }

    // {
    //     auto xxx = repr<add<"1", "9">()>();
    //     std::cout << "size: " << xxx.size() << std::endl;

    //     for (auto x : xxx) {
    //         std::cout << "char: " << x << std::endl;
    //     }
    // }

    // {
    //     auto xxx = std::to_array(ct_str("123").data);
    //     std::cout << "size: " << xxx.size() << std::endl;

    //     for (auto x : xxx) {
    //         std::cout << "char: " << x << std::endl;
    //     }
    // }

}