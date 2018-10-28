#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <inttypes.h>
#include <string.h>

extern "C" int main_c(int argc, char *argv[]);
extern "C" void Set_Color(int32_t group, uint32_t color); // 0xWWRRGGBB

#define LOCAL_SERVER_PORT 12551

int32_t socketReceive_UDP = -1;
struct sockaddr_in cliAddr, servAddr;
const int y = 1;

int main(int argc, char *argv[])
{
	if (setuid(0)) {
	perror("setuid");
	std::cerr << argv[0] << " must be run as root." << std::endl;
	return 1;
	}

    /* Socket erzeugen */
	socketReceive_UDP = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socketReceive_UDP < 0) {
	 printf ("%s: Kann Socket nicht öffnen ...(%s)\n",
	    argv[0], strerror(errno));
	 exit (EXIT_FAILURE);
	}

	/* Lokalen Server Port bind(en) */
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl (INADDR_ANY);  // INADDR_ANY 0x7f000001L
	servAddr.sin_port = htons (LOCAL_SERVER_PORT);
	setsockopt(socketReceive_UDP, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));

	int32_t rc = bind (socketReceive_UDP, (struct sockaddr *) &servAddr, sizeof (servAddr));

	if (rc < 0) {
	 printf ("%s: Kann Portnummern %d nicht binden (%s)\n",
	    argv[0], LOCAL_SERVER_PORT, strerror(errno));
	 exit (EXIT_FAILURE);
	}

	main_c(argc, argv);

	#define BUF 255
	char puffer[BUF];
	int32_t n = -1;
	uint32_t len = -1;
	time_t time1;
	char loctime[BUF];
	char *ptr;

    while (1) {
	    /* Puffer initialisieren */
	    memset (puffer, 0, BUF);
	    /* Nachrichten empfangen */
	    len = sizeof (cliAddr);
	    n = recvfrom (socketReceive_UDP, puffer, BUF, MSG_DONTWAIT,
	                   (struct sockaddr *) &cliAddr, &len );

	    if (-1 == n) 
	    {
	      sleep(0);
	    }
	    else if (n > 0) { 
		      /* Zeitangaben präparieren */
		      time(&time1);
		      strncpy(loctime, ctime(&time1), BUF);
		      ptr = strchr(loctime, '\n' );
		      *ptr = '\0';

		      /* Erhaltene Nachricht ausgeben */
		      printf ("%s: Daten erhalten von %s:UDP%u : %u : %s\n",
		              loctime, inet_ntoa (cliAddr.sin_addr),
		              ntohs (cliAddr.sin_port), n, puffer);


		      std::string givenString;
		      givenString.assign(puffer);

			  std::string searchStringGroupPos = "{\"group\":[";

			  std::string searchStringGroupPosEnd = "],\"color\":";

			  int32_t groupPos = givenString.find(searchStringGroupPos) + searchStringGroupPos.size();

			  int32_t groupPosEnd = givenString.find(searchStringGroupPosEnd);

			  std::string groupString = givenString.substr(groupPos, groupPosEnd - groupPos);

			  int32_t value_Group = std::stoi(groupString);

			  int32_t colorPosEnd = givenString.find("],\"command\":\"color\"");

			  std::string searchstringColorPos = ",\"color\":[";

			  int32_t colorPos = givenString.find(searchstringColorPos) + searchstringColorPos.size();

			  if (0 < colorPosEnd)
			  {
				  std::string valueString = givenString.substr(colorPos, colorPosEnd - colorPos);

				  std::string valueString_R = valueString.substr(0, valueString.find_first_of(","));

				  std::string valueString_G = valueString.substr(valueString.find_first_of(",") + 1, valueString.find_last_of(",") - valueString.find_first_of(",") - 1);

				  std::string valueString_B = valueString.substr(valueString.find_last_of(",") + 1, valueString.size() - valueString.find_last_of(","));

				  int32_t value_R = std::stoi(valueString_R);

				  int32_t value_G = std::stoi(valueString_G);

				  int32_t value_B = std::stoi(valueString_B);

				  uint32_t value = (((uint32_t)value_B) << 16)
				      + (((uint32_t)value_G) << 8)
				      + (((uint32_t)value_R) << 0);

			      Set_Color(value_Group, value);
			  }
			  else
			  {
			      Set_Color(value_Group, 0);
			  }
	    }
	}

	Set_Color(-1, 0x00123456);

	return 0;
}