#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "network.h"
#include "rtp.h"

static void printUsage() {
    fprintf(stderr, "Usage:  rtp-client host port\n\n");
    fprintf(stderr, "   example ./rtp-client localhost 4000\n\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {

    char message_1[] = "Gur hfr bs PBOBY pevccyrf gur zvaq; vgf "
                             "grnpuvat fubhyq, gurersber, or ertneqrq nf n pevzvany "
                             "bssrafr. -- Rqftne Qvwxfgen";

    char message_2[] = "P znxrf vg rnfl gb fubbg lbhefrys va gur "
                             "sbbg; P++ znxrf vg uneqre, ohg jura lbh qb, vg oybjf njnl "
                             "lbhe jubyr yrt. -- Ownear Fgebhfgehc";

    char message_3[] = "Va gur shgher, pbzchgref znl jrvtu ab zber guna 1.5 gbaarf. "
                             "-- Cbchyne zrpunavpf, 1949";

    char message_4[] = "Tebir tvirgu naq Tngrf gnxrgu njnl. "
                             "-- Obo Zrgpnysr (vairagbe bs Rgurearg) ba gur geraq bs "
                             "uneqjner fcrrqhcf abg orvat noyr gb xrrc hc jvgu fbsgjner qrznaqf";

    char message_5[] = "Pbqr trarengvba, yvxr qevaxvat nypbuby, vf tbbq va zbqrengvba. "
                             "-- Nyrk Ybjr";


    char *rcv_buffer;
    int length, ret;
    rtp_connection_t *connection;

    if (argc < 3) {
        printUsage();
        return EXIT_FAILURE;
    }

    if ((connection = rtp_connect(argv[1], atoi(argv[2]))) == NULL) {
        printUsage();
        return EXIT_FAILURE;
    }

    printf("Sending quotes to a remote host to have them "
                   "decoded using ROT13 cipher!\n\n");

    rtp_send_message(connection, message_1, strlen(message_1));
    ret = rtp_recv_message(connection, &rcv_buffer, &length);
    if (rcv_buffer == NULL || ret == -1) {
        printf("Connection reset by peer.\n");
        return EXIT_FAILURE;
    }
    printf("%.*s\n\n", length, rcv_buffer);
    free(rcv_buffer);

    rtp_send_message(connection, message_2, strlen(message_2));
    ret = rtp_recv_message(connection, &rcv_buffer, &length);
    if (rcv_buffer == NULL || ret == -1) {
        printf("Connection reset by peer.\n");
        return EXIT_FAILURE;
    }
    printf("%.*s\n\n", length, rcv_buffer);
    free(rcv_buffer);

    rtp_send_message(connection, message_3, strlen(message_3));
    ret = rtp_recv_message(connection, &rcv_buffer, &length);
    if (rcv_buffer == NULL || ret == -1) {
        printf("Connection reset by peer.\n");
        return EXIT_FAILURE;
    }
    printf("%.*s\n\n", length, rcv_buffer);
    free(rcv_buffer);

    rtp_send_message(connection, message_4, strlen(message_4));
    ret = rtp_recv_message(connection, &rcv_buffer, &length);
    if (rcv_buffer == NULL || ret == -1) {
        printf("Connection reset by peer.\n");
        return EXIT_FAILURE;
    }
    printf("%.*s\n\n", length, rcv_buffer);
    free(rcv_buffer);

    rtp_send_message(connection, message_5, strlen(message_5));
    ret = rtp_recv_message(connection, &rcv_buffer, &length);
    if (rcv_buffer == NULL || ret == -1) {
        printf("Connection reset by peer.\n");
        return EXIT_FAILURE;
    }
    printf("%.*s\n\n", length, rcv_buffer);
    free(rcv_buffer);
    rtp_disconnect(connection);
    return EXIT_SUCCESS;
}
