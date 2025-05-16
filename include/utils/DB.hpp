// #pragma once

// #include <tuple>
// #include <cstddef>
// #include <vector>
// #include <pqxx/pqxx>
// #include <string>

// using namespace std;

// class DB
// {
// public:
//     static pqxx::result exec_params_dynamic(pqxx::work &txn, const std::string &query, const std::vector<std::string> &params)
//     {
//         switch (params.size())
//         {
// #define CASE(N) \
//     case N:     \
//         return exec_params_from_vector<N>(txn, query, params);
//             CASE(1)
//             CASE(2)
//             CASE(3)
//             CASE(4)
//             CASE(5)
//             CASE(6)
//             CASE(7)
//             CASE(8)
//             CASE(9)
//             CASE(10)
//             CASE(11)
//             CASE(12)
//             CASE(13)
//             CASE(14)
//             CASE(15)
//             CASE(16)
//             CASE(17)
//             CASE(18)
//             CASE(19)
//             CASE(20)
//             CASE(21)
//             CASE(22)
//             CASE(23)
//             CASE(24)
//             CASE(25)
//             CASE(26)
//             CASE(27)
//             CASE(28)
//             CASE(29)
//             CASE(30)
//             CASE(31)
//             CASE(32)
//             CASE(33)
//             CASE(34)
//             CASE(35)
//             CASE(36)
//             CASE(37)
//             CASE(38)
//             CASE(39)
//             CASE(40)
//             CASE(41)
//             CASE(42)
//             CASE(43)
//             CASE(44)
//             CASE(45)
//             CASE(46)
//             CASE(47)
//             CASE(48)
//             CASE(49)
//             CASE(50)
//             CASE(51)
//             CASE(52)
//             CASE(53)
//             CASE(54)
//             CASE(55)
//             CASE(56)
//             CASE(57)
//             CASE(58)
//             CASE(59)
//             CASE(60)
// #undef CASE
//         default:
//             throw std::runtime_error("Unsupported parameter count (must be between 1 and 60)");
//         }
//     }

//     template <typename T, size_t... I>
//     static std::tuple<T> vectorToTuple(const std::vector<T> &vec, std::index_sequence<I...>)
//     {
//         return std::make_tuple(vec[I]...);
//     }

//     template <typename T>
//     static std::tuple<T> vectorToTuple(const std::vector<T> &vec)
//     {
//         return vectorToTuple<T>(vec, std::make_index_sequence<vec.size()>{});
//     }

//     template <typename... Args>
//     static pqxx::result exec_params_from_vector(pqxx::work &txn, const std::string &query, const std::vector<std::string> &params)
//     {
//         if (params.size() != sizeof...(Args))
//         {
//             throw std::runtime_error("Parameter count mismatch");
//         }

//         auto tup = vectorToTuple(params, std::make_index_sequence<sizeof...(Args)>{});

//         return std::apply([&](auto &&...args)
//                           { return txn.exec_params(query, args...); }, tup);
//     }
// };