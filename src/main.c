#include <stdio.h>
#include <kv.h>

int main() {
    kv_t *table = kv_init(1024);
    if (!table) return -1;

    printf("%p\n", table);
    printf("%ld\n", table->capacity);

    kv_put(table, "hehe", "haha");
    kv_put(table, "hehe", "hoho");
    kv_put(table, "lala", "baba");

    char *val = kv_get(table, "hehe");
    char *val2 = kv_get(table, "lala");
    char *val3 = kv_get(table, "This_doesn't_exit");

    printf("%s %s %s\n", val, val2, val3);

    kv_delete(table, "hehe");
    val = NULL;
    val = kv_get(table, "hehe");

    printf("%s %s %s\n", val, val2, val3);

    // Print all entries in db store
    // [TODO] Update to print TOMBSTONE
    // for (int i = 0; i < table->capacity; i++) {
    //     if (table->entries[i].key) {
    //         printf(
    //             "[%d] %s: %s\n",
    //             i,
    //             table->entries[i].key,
    //             table->entries[i].value
    //         );
    //     }
    // }
}
