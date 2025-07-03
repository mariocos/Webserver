#include "includes/webserv.hpp"

void	printLog(std::string action, ServerBlock *serverBlock, Client *client, Response *response, int mode)
{
	switch (mode)
	{
		case 0:
			std::cout<<"["<<getTimeStamp()<<"]"<<GREEN<<" ["<<action<<"] "<<RESET;
			std::cout<<GREEN<<serverBlock->getBlockName()<<" started on port "<<serverBlock->getBlockPort()<<RESET<<std::endl;
			break;
		case 1:
			std::cout<<"["<<getTimeStamp()<<"]"<<GREEN<<" ["<<action<<"] "<<RESET;
			//set which file was used to load the ServerBlock
			std::cout<<GREEN<<"Loaded configuration from default.config"<<RESET<<std::endl;
			break;
		case 2:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<serverBlock->getBlockName()<<" shutting down."<<RESET<<std::endl;
			break;
		case 3:
			std::cout<<"["<<getTimeStamp()<<"]"<<GREEN<<" ["<<action<<"] "<<RESET;
			std::cout<<GREEN<<"New connection from " + client->getClientIp() + ":" + transformToString(client->getClientPort())<<RESET<<std::endl;
			break;
		case 4:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"Disconnected "<<client->getClientIp() + ":" + transformToString(client->getClientPort())<<RESET<<std::endl;
			break;
		case 5:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<client->getClientIp() + ":" + transformToString(client->getClientPort()) + " - " + client->getClientRequest()->get_method();
			std::cout<<" "<<response->getPath()<<" "<<response->getStatusCode()<<RESET<<std::endl;
			break;
		case 6:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<"Served static file: "<<response->getPath()<<" (size: "<<client->getClientFile()->getFileStats()->st_size<<"KB)"<<RESET<<std::endl;
			break;
		case 7:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<"Executing script: "<<client->getClientRequest()->get_path()<<RESET<<std::endl;
			break;
		case 8:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<"Completed "<<client->getClientRequest()->get_path()<<" with status "<<response->getStatusCode()<<RESET<<std::endl;
			break;
		case 9:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<client->getClientRequest()->get_method() + " " + client->getClientRequest()->get_path() + " "<<response->getStatusCode();
			std::cout<<" from "<<client->getClientIp() + ":" + transformToString(client->getClientPort()) + " - " + client->getClientRequest()->get_content_length() + " bytes received"<<RESET<<std::endl;
			break;
		case 10:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<client->getClientRequest()->get_method() + " " + client->getClientRequest()->get_path() + " "<<response->getStatusCode();
			std::cout<<" from "<<client->getClientIp() + ":" + transformToString(client->getClientPort())<<RESET<<std::endl;
			break;
		case 11:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<"Sent "<<response->getBytesSent()<<"KB of "<<client->getClientFile()->getFileStats()->st_size<<"KB"<<RESET<<std::endl;
			break;
		case 12:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"TimedOut "<<client->getClientIp() + ":" + transformToString(client->getClientPort())<<RESET<<std::endl;
			break;
		case 13:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<client->getClientIp() + ":" + transformToString(client->getClientPort()) + " - " + client->getClientRequest()->get_method();
			std::cout<<" " + client->getClientRequest()->get_path() + " " + transformToString(response->getStatusCode())<<RESET<<std::endl;
			break;
		case 14:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<"Generated directory listing for: "<<client->getRouteTriggered()->getSavedPath()<<RESET<<std::endl;
			break;
		case 15:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"Error while sending to " + client->getClientIp() + ":" + transformToString(client->getClientPort())<<RESET<<std::endl;
			break;
		case 301:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<"Redirected permanently "<<client->getClientIp() + ":" + transformToString(client->getClientPort());
			std::cout<<" to: "<<client->getRouteTriggered()->getRedirectPath()<<RESET<<std::endl;
			break;
		case 307:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<"Redirected temporarily "<<client->getClientIp() + ":" + transformToString(client->getClientPort());
			std::cout<<" to: "<<client->getRouteTriggered()->getRedirectPath()<<RESET<<std::endl;
			break;
		case 400:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"Bad Request "<<response->getStatusCode()<<RESET<<std::endl;
			break;
		case 403:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"Forbidden "<<response->getStatusCode()<<RESET<<std::endl;
			break;
		case 404:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"File Not Found "<<response->getStatusCode()<<" "<<response->getPath()<<RESET<<std::endl;
			break;
		case 405:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"Method Not Allowed "<<response->getStatusCode()<<" - "<<client->getClientRequest()->get_method()<<RESET<<std::endl;
			break;
		case 409:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"Conflict "<<response->getStatusCode()<<" - "<<client->getClientRequest()->get_path()<<RESET<<std::endl;
			break;
		case 413:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"Payload Too Large "<<response->getStatusCode()<<" "<<response->getPath()<<RESET<<std::endl;
			break;
		case 503:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<client->getClientIp() + ":" + transformToString(client->getClientPort());
			std::cout<<" - 503 Webserver Busy"<<RESET<<std::endl;
			break;
		default:
			break;
	}
}
