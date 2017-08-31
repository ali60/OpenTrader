//#include "features.h"

//#ifdef _WIN32

#include <aws/core/Aws.h>
#include <aws/sqs/SQSClient.h>
#include <aws/sqs/model/ReceiveMessageRequest.h>
#include <aws/sqs/model/ReceiveMessageResult.h>
#include <aws/sqs/model/DeleteMessageRequest.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/DynamoDBRequest.h>
#include <aws/dynamodb/model/BatchGetItemRequest.h>
#include <aws/dynamodb/model/BatchWriteItemRequest.h>
#include <aws/dynamodb/model/CreateTableRequest.h>
#include <aws/dynamodb/model/DeleteItemRequest.h>
#include <aws/dynamodb/model/DeleteTableRequest.h>
#include <aws/dynamodb/model/DescribeLimitsRequest.h>
#include <aws/dynamodb/model/DescribeTableRequest.h>
#include <aws/dynamodb/model/DescribeTimeToLiveRequest.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <aws/dynamodb/model/ListTablesRequest.h>
#include <aws/dynamodb/model/ListTagsOfResourceRequest.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/QueryRequest.h>
#include <aws/dynamodb/model/ScanRequest.h>
#include <aws/dynamodb/model/TagResourceRequest.h>
#include <aws/dynamodb/model/UntagResourceRequest.h>
#include <aws/dynamodb/model/UpdateItemRequest.h>
#include <aws/dynamodb/model/UpdateTableRequest.h>
#include <aws/dynamodb/model/UpdateTimeToLiveRequest.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>
#include <aws/core/utils/threading/Executor.h>
#include <iostream>
#include <map>
#include <iterator>
#include "Job_queue.h"

using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::DynamoDB;
using namespace Aws::DynamoDB::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Json;

Aws::SDKOptions options;


Job_queue::Job_queue()
{
	queue_url = "https://sqs.ap-southeast-2.amazonaws.com/116127515664/OpenTraderQueue";
	Aws::InitAPI(options);
}


Job_queue::~Job_queue()
{
	Aws::ShutdownAPI(options);
}


void Job_queue::init()
{

}

bool Job_queue::get_next_job(void)
{
	Aws::Client::ClientConfiguration client_cfg;
	client_cfg.requestTimeoutMs = 30000;
	client_cfg.region = "ap-southeast-2";

	Aws::SQS::SQSClient sqs(client_cfg);

	Aws::SQS::Model::ReceiveMessageRequest rm_req;
	rm_req.SetQueueUrl(queue_url.c_str());
	rm_req.SetMaxNumberOfMessages(1);

	auto rm_out = sqs.ReceiveMessage(rm_req);
	if (!rm_out.IsSuccess()) {
		std::cout << "Error receiving message from queue " << queue_url << ": "
			<< rm_out.GetError().GetMessage() << std::endl;
		return false;
	}

	const auto& messages = rm_out.GetResult().GetMessages();
	if (messages.size() == 0) {
		std::cout << "No messages received from queue " << queue_url <<
			std::endl;
		return false;
    }

	const auto& message = messages[0];
	std::cout << "Received message:" << std::endl;
	std::cout << "  MessageId: " << message.GetMessageId() << std::endl;
	queue_id_ = message.GetMessageId();
	std::cout << "  ReceiptHandle: " << message.GetReceiptHandle() << std::endl;
	std::cout << "  Body: " << message.GetBody() << std::endl << std::endl;
	parse_queue_message(message.GetBody());
	return true;
}


//message structure
//from_date=YYYYMMDD
//todate=YYYYMMDD
//symbol=EURUSD
//indicator=ema_cross,stochastic,rsi,macd
/*
from_date=20170101
to_date=20170401
symbol=EURUSD
indicator=ema_cross
*/
void Job_queue::parse_queue_message(std::string message_body)
{
	std::stringstream ss(message_body);
	std::string line,key,value;
	queue_dic_.empty();
	while (std::getline(ss, line))
	{
		auto i = line.find('=');
		if (i == -1)
			continue;
		key = line.substr(0, i);
		value = line.substr(i + 1, line.size() - i);
		queue_dic_.insert(std::make_pair(key, value));
	}
}



void Job_queue::store_result_db(std::map < std::string, std::string> result_dic)
{
	Aws::Client::ClientConfiguration client_cfg;
	client_cfg.requestTimeoutMs = 30000;
	client_cfg.region = "ap-southeast-2";
	Aws::DynamoDB::DynamoDBClient dynamoDbClient(client_cfg);
	PutItemRequest putItemRequest;
	putItemRequest.WithTableName("Results");
	AttributeValue idAttribute;
	idAttribute.SetS(queue_id_);
	putItemRequest.AddItem("Id", idAttribute);
	for (auto item : result_dic) 
	{
		AttributeValue paramsAttribute;
		paramsAttribute.SetS(item.second);
		putItemRequest.AddItem(item.first, paramsAttribute);
	}

	auto putItemOutcome = dynamoDbClient.PutItem(putItemRequest);

	if (putItemOutcome.IsSuccess())
	{
		std::cout << "PutItem Success Using IOPS " << std::endl;
	}
	else
	{
		std::cout << "PutItem failed with error " << putItemOutcome.GetError().GetMessage()<<std::endl;
	}

}

//#endif //_SUPPORT_SQS
