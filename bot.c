#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>

#include "discord.h"
#include "guild.h"

#define FILENAME "jokes.txt"
#define PREFIX "$"

char** lines_global;
int num_global;

typedef struct
{
    int number;
    int length;
} fileSizes;

void openFile(FILE** file, char* name)
{
    *file = fopen(name, "r");

    if (*file == NULL)
        printf("file can't be opened \n");
}

fileSizes fileSizeOf(FILE** file)
{
    int lines = 0;            // number of arrays
    int max_line_length = 0;  // length of arrays

    int curr_line_length = 0; // helper value
    char ch;                  // current char
    while (!feof(*file))
    {
        ch = fgetc(*file);
        if (ch != '\n')
            curr_line_length++;
        else
        {
            lines++;
            if (curr_line_length > max_line_length)
                max_line_length = curr_line_length;
            curr_line_length = 0;
        }
    }

    num_global = lines;

    return (fileSizes){.number = lines, .length = max_line_length};
}

char** loadLines(FILE** file, int num, int length)
{
    char** lines = malloc(sizeof(char*) * num);

    int i = 0;
    while (!feof(*file))
    {
        lines[i] = malloc(length);
        fgets(lines[i], length, *file);
        i++;
    }
    lines_global = lines;
    return lines;
}

void randomLine(struct discord *client, const struct discord_message *event)
{
    if (event->author->bot) return;
    int r = rand() % num_global;
    struct discord_create_message params = { .content = lines_global[r] };
    discord_create_message(client, event->channel_id, &params, NULL);
}

void on_message_create(struct discord *client, const struct discord_message *event)
{
    if (event->author->bot) return;
    int c = (rand() % 10) + 1;
    if (c % 4 != 0) return; // 25% of the time

    int r = rand() % num_global;
    struct discord_create_message params = { .content = lines_global[r] };
    discord_create_message(client, event->channel_id, &params, NULL);
}

void print_usage(void)
{
    printf(
        "\n\nThis bot asynchronously posts jokes.\n"
        "Jokes and time frames configurable.\n"
        "This bot is under development.\n"
        "\nTYPE ANY KEY TO START BOT\n");
}

void on_ready(struct discord *client, const struct discord_ready *event)
{
    log_info("ryebot succesfully connected to Discord as %s#%s!",
             event->user->username, event->user->discriminator);

    struct discord_activity activities[] = {
        {
            .name = "loafing around",
            .type = DISCORD_ACTIVITY_GAME,
            .details = "a toast to you!",
        },
    };

    struct discord_presence_update status = {
        .activities =
            &(struct discord_activities){
                .size = sizeof(activities) / sizeof *activities,
                .array = activities,
            },
        .status = "online",
        .afk = false,
        .since = discord_timestamp(client),
    };

    discord_update_presence(client, &status);
}

void on_reaction_add(struct discord *client, const struct discord_message_reaction_add *event)
{
    if (event->member->user->bot)
        return;

    discord_create_reaction(client, event->channel_id, event->message_id, event->emoji->id, event->emoji->name, NULL);
}

void timed_message(struct discord *client, struct discord_timer* timer)
{
    //printf("###TEST TEST TEST###");

    int r = rand() % num_global;
    struct discord_create_message params = { .content = lines_global[r] };
    discord_create_message(client, 769381887608619009, &params, NULL); // hardcoded currently to channel, should move to config file
}

void on_reaction_remove(struct discord *client, const struct discord_message_reaction_remove *event)
{
    int r = rand() % num_global;
    struct discord_create_message params = { .content = lines_global[r] };
    discord_create_message(client, event->channel_id, &params, NULL);
}

int main(int argc, char *argv[])
{
    const char *config_file;
    if (argc > 1)
        config_file = argv[1];
    else
        config_file = "../config.json";

    ccord_global_init();
    struct discord *client = discord_config_init(config_file);
    assert(NULL != client && "Couldn't initialize client");

    srand(time(NULL));
    FILE* ptr;
    openFile(&ptr, FILENAME);

    fileSizes sizes = fileSizeOf(&ptr);
    printf("Read %d lines\n", sizes.number);
    printf("Longest line %d\n", sizes.length);
    rewind(ptr);

    sizes.length+=2; // maybe this caused the corrupted size vs prev size error?
    char** jokes = loadLines(&ptr, sizes.number, sizes.length);

    discord_timer_interval(client, timed_message, NULL, 0, 3600000, -1);
    discord_set_on_ready(client, &on_ready);
    discord_set_prefix(client, PREFIX);
    discord_set_on_message_reaction_remove(client, &on_reaction_remove);
    discord_set_on_message_reaction_add(client, &on_reaction_add);
    discord_set_on_message_create(client, &on_message_create);
    discord_set_on_command(client, "random", &randomLine);

    print_usage();
    fgetc(stdin); // wait for input

    discord_run(client);

    discord_cleanup(client);

    ccord_global_cleanup();
    // cleanup
    fclose(ptr);
    free(jokes);
    return 0;
}
