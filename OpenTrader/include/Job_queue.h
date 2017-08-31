#ifndef _OPEN_TRADER_JOB_QUEUE_INC_
#define _OPEN_TRADER_JOB_QUEUE_INC_


class Job_queue
{
	std::map < std::string, std::string> queue_dic_;
	std::string  queue_id_;
public:
	Job_queue();
	virtual ~Job_queue();
	bool get_next_job();
	std::string queue_url;
	void init();
	void Job_queue::store_result_db(std::map < std::string, std::string> result_dic);
	void parse_queue_message(std::string message_body);
	std::map < std::string, std::string> get_message_dic() { return queue_dic_; };

};


#endif
