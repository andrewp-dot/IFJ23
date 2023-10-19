#include "symtable.h"

void symtable_init(symtab_t *symtab)
{
    symtab->size = 11;
    symtab->count = 0;
    symtab->deactivated = 0;

    symtab->items = malloc(sizeof(symtab_item_t) * symtab->size);

    for (size_t i = 0; i < symtab->size; i++)
        symtab->items[i] = NULL;
}

/**
 * @brief implementation of djb2 hash
 *
 * @param id
 * @return unsigned long
 */
unsigned long hash(char *id, size_t size)
{
    uint32_t hash = 0;
    const unsigned char *p;

    for (p = (const unsigned char *)id; *p != '\0'; p++)
        hash = 65599 * hash + *p;
    return (hash % size);
}

unsigned long hash2(char *id, size_t size)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *id++))
        hash = ((hash << 5) + hash) + c;

    return (hash % (size - 1)) + 1;
}

unsigned long get_hash(dstring_t *id, symtab_item_t **items, size_t size)
{
    char *wanted = dstring_to_str(id);

    unsigned long index = hash(wanted, size);
    unsigned long step = hash2(wanted, size);

    for (int i = 0; items[index] != NULL; i++) // gets hash of 1st null slot
        index = (index + i * step) % size;     // if occupied, double hash

    return index;
}

symtab_item_t *symtable_search(symtab_t *symtab, dstring_t *id)
{
    char *wanted = dstring_to_str(id);

    unsigned long index = hash(wanted, symtab->size);
    unsigned long step = hash2(wanted, symtab->size);

    if (symtab == NULL)
        return NULL;

    for (int i = 0; symtab->items[index] != NULL; i++)
    {
        if (!dstring_cmp(&(symtab->items[index])->name, id))
            if ((symtab->items[index])->active) // if name matches and item is active,g success
                return (symtab->items[index]);
            else // if item is inactive, it was deleted, not found
                return NULL;
        else
            index = (index + i * step) % symtab->size;
    }
    return NULL;
}

bool is_in_symtbale(symtab_t *symtab, dstring_t *id)
{
    return (symtable_search(symtab, id) != NULL);
}

symtab_item_t *item_init(dstring_t *id, bool *err)
{
    symtab_item_t *new = malloc(sizeof(symtab_item_t));

    if (!new)
    {
        *err = true;
        return NULL;
    }
    new->active = true;
    dstring_init(&new->name);
    new->type = undefined;
    new->is_mutable = false;
    new->is_func_defined = false;
    new->is_var_declared = false;
    new->parametrs = NULL;
    new->return_type = undefined;
    new->local_symtable = NULL;

    dstring_copy(id, &new->name);
    return new;
}

void resize(symtab_t *symtab)
{
    const size_t primes[] = {11, 23, 53, 107, 211, 421, 853, 1699, 3209, 6553, 12409, 25229};

    size_t new_size;
    for (int i = 0; primes[i]; i++)
    {
        if (symtab->size < primes[i])
        {
            new_size = primes[i];
            break;
        }
    }

    DEBUG_PRINT("resizing table to %d", primes[i])

    symtab_item_t **resized_items = malloc(sizeof(symtab_item_t) * new_size);

    for (size_t i = 0; i < new_size; i++)
        resized_items[i] = NULL;

    for (size_t i = 0; i < symtab->size; i++)
    {
        if (symtab->items[i] && symtab->items[i]->active)
        {
            uint64_t new_hash = get_hash(&symtab->items[i]->name, resized_items, new_size);
            resized_items[new_hash] = symtab->items[i];
        }
    }

    free(symtab->items);

    symtab->items = resized_items;
    symtab->size = new_size;
    symtab->count -= symtab->deactivated;
    symtab->deactivated = 0;
}

void check_load(symtab_t *symtab)
{
    if (0.65 < (float)((float)symtab->count / (float)symtab->size))
        resize(symtab);
}

uint8_t symtable_insert(symtab_t *symtab, dstring_t *id)
{
    symtab_item_t *item = symtable_search(symtab, id); // try to search in symtab
    if (!item)                                         // if not in symtab alloc new slot for data
    {
        bool error = false;
        symtab->items[get_hash(id, symtab->items, symtab->size)] = item_init(id, &error); // handover poiter to new allocated item
        if (error)
            return ERR_INTERNAL;
        symtab->count++;
        check_load(symtab);
    }
    else
        return 1;

    return 0;
}

uint8_t symtable_delete(symtab_t *symtab, dstring_t *target)
{
    symtab_item_t *item = symtable_search(symtab, target);
    if (!item)
        return 1;

    item->active = false;
    symtab->deactivated++;
    return 0;
}
void param_dispose(param_t *first)
{
    while (first)
    {
        param_t *temp = first->next;
        dstring_free(&first->name);
        dstring_free(&first->label);
        free(first);

        first = temp;
    }
}

void symtable_dispose(symtab_t *symtab)
{
    for (size_t i = 0; i < symtab->size; ++i)
    {
        if (symtab->items[i] != NULL) // free only allocated pointer, not null pointers
        {
            dstring_free(&symtab->items[i]->name);

            param_dispose(symtab->items[i]->parametrs);

            free(symtab->items[i]);
            symtab->items[i] = NULL;
        }
    }
    free(symtab->items);
}

uint8_t set_local_symtable(symtab_t *global_symtab, dstring_t *func_id, symtab_t *local_symtab)
{
    symtab_item_t *item = symtable_search(global_symtab, func_id);
    if (!item)
        return 1;
    if (item->type != function)
        return 2;
    item->local_symtable = local_symtab;
    return 0;
}

symtab_t *get_local_symtable(symtab_t *global_symtab, dstring_t *func_id, bool *err)
{
    symtab_item_t *item = symtable_search(global_symtab, func_id);
    if (!item)
    {
        *err = false;
    }
    else if (item->type != function)
    {
        *err = true;
    }
    else
    {
        *err = false;
        return item->local_symtable;
    }
    return NULL;
}

uint8_t set_type(symtab_t *symtab, dstring_t *id, Type type)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 1;
    item->type = type;
    return 0;
}

Type get_type(symtab_t *symtab, dstring_t *id, bool *err)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (item == NULL)
    {
        *err = true;
        return undefined;
    }
    else
    {
        *err = false;
        return item->type;
    }
}

uint8_t set_mutability(symtab_t *symtab, dstring_t *id, bool is_mutable)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 1;
    if (item->type == function)
        return 2;
    item->is_mutable = is_mutable;
    return 0;
}

bool get_mutability(symtab_t *symtab, dstring_t *id, bool *err)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item || item->type == function)
    {
        *err = true;
        return false;
    }
    else
    {
        *err = false;
        return item->is_mutable;
    }
}

uint8_t set_func_definition(symtab_t *symtab, dstring_t *id, bool is_func_defined)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 1;
    if (item->type != function)
        return 2;
    item->is_func_defined = is_func_defined;
    return 0;
}

bool get_func_definition(symtab_t *symtab, dstring_t *id, bool *err)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item || item->type != function)
    {
        *err = true;
        return false;
    }
    else
    {
        *err = false;
        return item->is_func_defined;
    }
}

uint8_t set_var_declaration(symtab_t *symtab, dstring_t *id, bool is_var_declared)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 1;
    if (item->type == function)
        return 2;
    item->is_var_declared = is_var_declared;
    return 0;
}

bool get_var_declaration(symtab_t *symtab, dstring_t *id, bool *err)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item || item->type == function)
    {
        *err = true;
        return false;
    }
    else
    {
        *err = false;
        return item->is_var_declared;
    }
}

uint8_t set_return_type(symtab_t *symtab, dstring_t *id, Type return_type)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 1;
    if (item->type != function)
        return 2;

    item->return_type = return_type;

    return 0;
}

Type get_return_type(symtab_t *symtab, dstring_t *id, bool *err)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item || item->type != function)
    {
        *err = true;
        return undefined;
    }
    else
    {
        *err = false;
        return item->return_type;
    }
}

param_t *param_init(dstring_t *name_of_param, bool *err)
{
    param_t *node = malloc(sizeof(param_t));
    if (!node)
    {
        *err = true;
        return NULL;
    }

    dstring_init(&node->name);
    dstring_copy(name_of_param, &node->name);
    dstring_init(&node->label);
    node->type = undefined;
    node->next = NULL;

    return node;
}

param_t *search_param(param_t *first, dstring_t *id)
{
    if (!first)
        return NULL;

    param_t *node = first;

    do
    {
        if (!dstring_cmp(&node->name, id))
            return node;
        node = node->next;
    } while (node);

    return NULL;
}

uint8_t add_param(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, bool *err)
{
    symtab_item_t *item = symtable_search(symtab, func_id);
    if (!item)
        return 1;
    if (item->type != function)
        return 2;
    if (!item->parametrs)
        item->parametrs = param_init(name_of_param, err);
    else
    {
        param_t *runner = item->parametrs;
        while (runner->next)
            runner = runner->next;

        runner->next = param_init(name_of_param, err);
        (runner->next)->next = NULL;
    }
    return 0;
}

uint8_t set_param_type(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, Type type)
{
    symtab_item_t *item = symtable_search(symtab, func_id);
    if (!item)
        return 1;
    if (item->type != function)
        return 2;
    if (!item->parametrs)
        return 3;
    param_t *node = search_param(item->parametrs, name_of_param);
    if (!node)
        return 3;
    node->type = type;
    return 0;
}

Type get_param_type(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, uint8_t *err)
{
    symtab_item_t *item = symtable_search(symtab, func_id);
    if (!item)
    {
        *err = 1;
        return undefined;
    }
    if (item->type != function)
    {
        *err = 2;
        return undefined;
    }
    if (!item->parametrs)
    {
        *err = 3;
        return undefined;
    }
    param_t *node = search_param(item->parametrs, name_of_param);
    if (!node)
    {
        *err = 3;
        return undefined;
    }
    *err = 0;
    return node->type;
}

uint8_t set_param_label(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, dstring_t *label)
{
    symtab_item_t *item = symtable_search(symtab, func_id);
    if (!item)
        return 1;
    if (item->type != function)
        return 2;
    if (!item->parametrs)
        return 3;
    param_t *node = search_param(item->parametrs, name_of_param);
    if (!node)
        return 3;
    dstring_copy(label, &node->label);
    return 0;
}

dstring_t *get_param_label(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, uint8_t *err)
{
    symtab_item_t *item = symtable_search(symtab, func_id);
    if (!item)
    {
        *err = 1;
        return undefined;
    }
    if (item->type != function)
    {
        *err = 2;
        return undefined;
    }
    if (!item->parametrs)
    {
        *err = 3;
        return undefined;
    }
    param_t *node = search_param(item->parametrs, name_of_param);
    if (!node)
    {
        *err = 3;
        return undefined;
    }
    *err = 0;
    return &node->label;
}