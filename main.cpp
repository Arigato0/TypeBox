#include <iostream>

#include "type_box.hpp"


int main()
{
	TypeBox type_box;

	type_box.emplace<std::string>("hello world");

	auto opt = type_box.fetch<std::string>();

	if (opt.has_value())
	{
		auto &funnel = opt.value();

		for (int i = 0; i < funnel.size(); i++)
		{
			std::cout << *funnel.next() << '\n';
		}
	}

	type_box.clear<std::string>();
}
