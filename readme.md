TypeBox is a mailbox for typed data.

This is a tiny demonstration for an idea I had about a data sharing and synchronization model.

The implementation is not great especially in terms of how it stores and allocates the data but for the purposes of this example it will do.

Take this example where you have two objects that may need to share data with each other but they cannot know of each other's existence
```c++

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

private:
	TypeBox &m_type_box;
};
    
```

In this example ObjectB does not know about ObjectA existing but will need some data such as the payload from time to time.

ObjectA can put such a payload inside the type box for any other object anywhere else to look at it or consume it entirely.

This creates a complete decoupling between systems that only need to share data.

This system could be further expanded upon by putting data inside different channels and even gating them off with permission models so when an object is given a type box they can be given a view of a typebox that stores permissions.

I might turn this into a library later on.