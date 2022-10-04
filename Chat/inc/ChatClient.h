#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

class ChatClient {
public:

	// Constructor
	ChatClient (const string& name);

	// Destructor
	virtual
	~ChatClient();

	// To run the chat client receive loop.  This runs
	// on a separate thread.
	void runReceiveLoop();

	// To run the chat client input loop.  This will
	// run on the same thread as the constructor.
	void runInputLoop();

private:
	// The name of this client
	string m_name;

	// The socket for this client.
	int m_socket;

	// To stop the running of this client
	bool m_shutdown;

	// A condition variable, used to start the receive loop
	std::condition_variable m_cv;

	// A mutex for m_cv
	std::mutex m_mutex;

	// The receive thread
	std::thread m_receiver;

};
