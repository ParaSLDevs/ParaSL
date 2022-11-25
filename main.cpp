#include <boost/spirit/include/qi.hpp>
#include <iostream>

namespace parasl {

using brac_tree = boost::make_recursive_variant<
        std::string, std::vector<boost::recursive_variant_> >::type;

namespace qi = boost::spirit::qi;

template<typename It, typename Skipper>
struct my_grammar : qi::grammar<It, brac_tree(), Skipper> {
    my_grammar() : my_grammar::base_type{brackets} {
        brackets = simple | nested;

        nested = '[' >> *brackets >> ']';

        simple = +~qi::char_("][");

        BOOST_SPIRIT_DEBUG_NODES((simple)(nested)(brackets))
    }

private:
    qi::rule<It, std::string(), Skipper> simple;
    qi::rule<It, std::vector<brac_tree>(), Skipper> nested;
    qi::rule<It, brac_tree(), Skipper> brackets;
};

}  // namespace parasl

namespace ascii = boost::spirit::qi::ascii;

int main(int argc, char *argv[]) {
    std::string storage;
    std::getline(std::cin, storage);

    const parasl::my_grammar<std::string::iterator, ascii::space_type> g;

    parasl::brac_tree ast; // Our tree
    std::string::iterator beg = storage.begin();
    std::string::iterator end = storage.end();
    bool res = phrase_parse(beg, end, g, ascii::space, ast);

    bool is_full_parsed = (beg == end);
    if (res && is_full_parsed) {
        std::cout << "Parsing succeeded" << "\n";
        return 0;
    } else {
        std::cerr << "Parsing failed\n";
        return 1;
    }
}
