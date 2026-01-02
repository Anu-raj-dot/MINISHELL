#include "header.h"

extern process *head;

void insert_at_first(pid_t Pid, char *input_string) // insert at first
{
    process *new = malloc(1 * sizeof(process));
    if (new == NULL)
    {
        printf("ERROR : Failed to create a node\n");
        return;
    }

    new->pid = Pid;
    strcpy(new->data, input_string);
    new->next = head;

    head = new;
    return;
}

void delete_pid_node(pid_t del_pid) // delete node based on pid
{
    process *temp = head;
    process *prev = NULL;
    while (temp)
    {
        if (temp->pid == del_pid)
        {
            if (head == temp)
            {
                head = temp->next;
            }
            else
            {
                prev->next = temp->next;
            }
            free(temp);
            return;
        }

        prev = temp;
        temp = temp->next;
    }
    return;
}

void delete_at_first(void) // delete first node
{
    if (head == NULL)
        return;

    process *temp = head;
    head = head->next;

    free(temp);
    return;
}

void print_stop_process(void) // printing nodes
{
    if (head == NULL)
    {
        return;
    }

    int i = 0;
    process *temp = head;
    while (temp != NULL)
    {
        printf("[%d][pid:%d]    Stopped    %s\n", ++i, temp->pid, temp->data);
        temp = temp->next;
    }
    return;
}
