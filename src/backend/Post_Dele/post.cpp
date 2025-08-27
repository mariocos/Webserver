#include "../../../includes/webserv.hpp"

int	create_and_submit(RequestParse req)//creates and submits the request
{
	std::ofstream	sub_file(req.get_path().c_str());
	if (!sub_file)
		return (-1);
	/* write header to file */
	sub_file << "Content-Type: " << req.get_content_type() << "\r\n";
	sub_file << "Content-Length: " << req.get_content_length() << "\r\n\r\n";
	sub_file << req.get_content();
	return(1);

}


//should return an apropriate exit code
int	post(RequestParse req)
{
	/*   check if file exists  */
	if (access(req.get_path().c_str(), F_OK))
	{
		std::cout << "not sure what to do if file already exists\n";
	/*    if file exists what do    */
	}
	else
	{
		create_and_submit(req);
	}



	/*   if file doesnt exits create and submit   */





	return(1);

}