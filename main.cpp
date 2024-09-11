#include <iostream>

#include "type_box.hpp"

struct Payload
{
	std::string name;
	int id;
	std::vector<double> values;
};

class ObjectA
{
public:
	// gets an instance of a type box through dependency injection
	ObjectA(TypeBox &type_box) :
		m_type_box(type_box)
	{}

	void do_work()
	{
		// do some work here

		// emplaces some data such as a payload into tye type box
		m_type_box.emplace<Payload>("my data", 42, std::vector{1.5, 2.39, 8.9});

		// do some more work
	}

private:
	TypeBox &m_type_box;
};

class ObjectB
{
public:
	// once again gets an instance of a type box through dependency injection
	ObjectB(TypeBox &type_box) :
		m_type_box(type_box)
	{}

	void do_work()
	{
		// do some work here

		auto opt = m_type_box.fetch<Payload>();

		if (opt.has_value())
		{
			auto &funnel = opt.value();

			for (int i = 0; i < funnel.size(); ++i)
			{
				process_payload(funnel.next());
			}
		}

		// do some more work
	}

	void process_payload(Payload *payload)
	{
		std::cout << payload->id << " " << payload->name << '\n';
	}

private:
	TypeBox &m_type_box;
};

int main()
{
	TypeBox type_box;

	ObjectA obja(type_box);
	ObjectB objb(type_box);

	obja.do_work();
	objb.do_work();
}
