/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgalle <davgalle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 16:28:33 by davgalle          #+#    #+#             */
/*   Updated: 2025/01/20 17:52:10 by davgalle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>

int count = 0, max_fd = 0;
int ids[65536];
char *msgs[65536];

char buf_read[1001], buf_write[42];

fd_set rfds, wfds, afds;

int extract_message(char **buf, char **str)
{
    char *aux;
    int i;

    *str = 0;
    if (*buf == 0)
        return (0);
    i = 0;
    while ((*buf)[i])
    {
        if ((*buf)[i] == '\n')
        {
            aux = calloc(1, sizeof(*aux) * (strlen(*buf + i + 1) + 1));
            if (!aux)
                return (-1);
            strcpy(aux, *buf + i + 1);
            *str = *buf;
            (*str)[i + 1] = 0;
            *buf = aux;
            return (1);
        }
        i++;
    }
    return (0);
}

char *str_join(char *buf, char *add)
{
    char *new;
    int len;

    if (buf == 0)
        len = 0;
    else
        len = strlen(buf);
    new = malloc(sizeof(*new) * (len + strlen(add) + 1));
    if (!new)
        return NULL;
    new[0] = 0;
    if (buf != 0)
        strcat(new, buf);
    free(buf);
    strcat(new, add);
    return(new);
}

void fatal_error()
{
    write(2, "Fatal error\n", 12);
    exit(1);
}

void notify_other(int author, char *str)
{
    for(int fd = 0; fd <= max_fd; fd++)
    {
        if (FD_ISSET(fd, &wfds) && fd != author)
            send(fd, str, strlen(str), 0);
    }
}

void register_client(int fd)
{
    max_fd = fd > max_fd ? fd : max_fd;
    ids[fd] = count++;
    msgs[fd] = NULL;
    FD_SET(fd, &afds);
    sprintf(buf_write, "server: client %d just arrived\n", ids[fd]);
    notify_other(fd, buf_write);
}

void remove_client(int fd)
{
    sprintf(buf_write, "server: client %d just left\n", ids[fd]);
    notify_other(fd, buf_write);
    free(msgs[fd]);
    FD_CLR(fd, &afds);
    close(fd);
}

void send_message(int fd)
{
    char *str;

    while (extract_message(&(msgs[fd]), &str))
    {
        sprintf(buf_write, "client %d:", ids[fd]);
        notify_other(fd, buf_write);
        notify_other(fd, str);
        free(str);
    }
}

int create_socket()
{
    max_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (max_fd < 0)
        fatal_error();
    FD_SET(max_fd, &afds);
    return(max_fd);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        write(2, "Wrong number of arguments\n", 26);
        exit(1);
    }

    FD_ZERO(&afds);
    int sockfd = create_socket();

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    servaddr.sin_port = htons(atoi(argv[1]));

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)))
        fatal_error();
    if (listen(sockfd, SOMAXCONN))
        fatal_error();
    
    while (1)
    {
        rfds = wfds = afds;
        if (select(max_fd + 1, &rfds, &wfds, NULL, NULL) < 0)
            fatal_error();

        for(int fd = 0; fd <= max_fd; fd++)
        {
            if (!FD_ISSET(fd, &rfds))
                continue;
            if (fd == sockfd)
            {
                socklen_t addrlen = sizeof(servaddr);
                int client_fd = accept(sockfd, (struct sockaddr *)&servaddr, &addrlen);
                if (client_fd >= 0)
                {
                    register_client(client_fd);
                    break;
                }
            }
            else
            {
                int bytes_read = recv(fd, buf_read, 1000, 0);
                if (bytes_read <= 0)
                {
                    remove_client(fd);
                    break;
                }
                buf_read[bytes_read] = '\0';
                msgs[fd] = str_join(msgs[fd], buf_read);
                send_message(fd);
            }
        }
    }
    return (0);
}
