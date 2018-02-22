#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>

#include "column.h"
#include "operators.h"

using namespace std;

void split(vector<experimental::string_view>& results, string const& original, char separator);

int main()
{
    std::vector<std::unique_ptr<Column>> columns;
    columns.push_back(std::make_unique<TypedColumn<PlainStore, Int64Type>>());
    columns.push_back(std::make_unique<TypedColumn<PlainStore, StringType>>());

    std::vector<std::shared_ptr<UnaryOperator>> casters2Type;
    casters2Type.push_back(std::make_shared<FromStringCast<Int64Type>>());
    casters2Type.push_back(std::make_shared<FromStringCast<StringType>>());

    std::vector<std::shared_ptr<UnaryOperator>> casters2String;
    casters2String.push_back(std::make_shared<ToStringCast<Int64Type>>());
    casters2String.push_back(std::make_shared<ToStringCast<StringType>>());

    chrono::time_point<std::chrono::high_resolution_clock> start, end;

    fstream input("/home/andrei/Desktop/MC5Dau.csv");
    string line;
    line.reserve(1024 * 1024);

    vector<experimental::string_view> piece;

    uint32_t counter = 0;
    {
        start = chrono::high_resolution_clock::now();

        getline(input, line);
        while(getline(input, line))
        {
            split(piece, line, ',');
            for(size_t i = 0; i < piece.size(); i++)
            {
                try {
                    experimental::string_view* value = &piece.at(i);
                    ViewByteBuffer val(value->size(), value->data());
                    ByteBuffer buffer = casters2Type.at(i)->operation(val);
                    columns.at(i)->put(buffer);
                } catch(exception& ex)
                {
                    cout << __FILE__ << __LINE__ << ex.what() << " " << line << endl;
                    abort();
                }
            }

            piece.clear();
            counter++;
        }

        end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed_time = end - start;

        cout << counter << " read duration = " << elapsed_time.count() << "s" << std::endl;
    }

//    ofstream out("/home/andrei/Desktop/output.csv");

//    {
//        start = chrono::high_resolution_clock::now();

//        for(uint64_t i = 0; i < counter; ++i)
//        {
//            for(uint64_t j = 0; j < (columns.size() - 1); ++j)
//            {
//                ViewByteBuffer val = columns.at(j)->getView(i);
//                out << casters2String.at(j)->operation(val) << ",";
//            }
//            ViewByteBuffer val = columns.at(columns.size() - 1)->getView(i);
//            out << casters2String.at(columns.size() - 1)->operation(val) << endl;
//        }

//        end = chrono::high_resolution_clock::now();
//        chrono::duration<double> elapsed_time = end - start;

//        cout << counter << " write duration = " << elapsed_time.count() << "s" << std::endl;
//    }

//    out.close();

    return 0;
}

void split(vector<experimental::string_view>& results, string const& original, char separator)
{
    string::const_iterator start = original.begin();
    string::const_iterator end = original.end();
    string::const_iterator next = find(start, end, separator);
    while(next != end && *start == '"' && *(next-1) != '"')
    {
        next = find(next + 1, end, separator);
    }
    while (next != end) {
        experimental::string_view str(start.operator ->(), static_cast<uint64_t>(next.operator ->() - start.operator ->()));
        results.push_back(str);

        start = next + 1;
        next = find(start, end, separator);
        while(*start == '"' && *(next-1) != '"' && next != end)
        {
            next = find(next + 1, end, separator);
        }
    }

    experimental::string_view str(start.operator ->(), static_cast<uint64_t>(next.operator ->() - start.operator ->()));
    results.push_back(str);
}
