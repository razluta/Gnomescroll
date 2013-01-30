#include "attributes.hpp"

#include <common/dat/properties.hpp>

namespace Attributes
{

class Attribute: public Property<AttributeType>
{
    public:
        AttributeGroup group;
        AttributeValueType value_type;
        AttributeSyncType sync_type;

        void* location;
        voidFunction getter;
        voidFunction setter;

        bool changed;
        ClientID sync_to;

    /* Read/write API */

    int get_int()
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_INT) return 1;
        if (this->location != NULL)
            return *(reinterpret_cast<int*>(this->location));
        return reinterpret_cast<getInt>(this->getter)();
    }

    float get_float()
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_FLOAT) return 1.0f;
        if (this->location != NULL)
            return *(reinterpret_cast<float*>(this->location));
        return reinterpret_cast<getFloat>(this->getter)();
    }

    const char* get_string()
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_STRING) return NULL;
        if (this->location != NULL)
            return *(reinterpret_cast<char**>(this->location));
        return reinterpret_cast<getString>(this->getter)();
    }

    void set(int value)
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_INT) return;
        if (this->location != NULL)
        {
            int current = *(reinterpret_cast<int*>(this->location));
            if (current != value)
            {
                *(reinterpret_cast<int*>(this->location)) = value;
                this->changed = true;
            }
        }
        else if (reinterpret_cast<setInt>(this->setter)(value))
            this->changed = true;
    }

    void set(float value)
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_FLOAT) return;
        if (this->location != NULL)
        {
            float current = *(reinterpret_cast<float*>(this->location));
            if (current != value)
            {
                *(reinterpret_cast<float*>(this->location)) = value;
                this->changed = true;
            }
        }
        else
        if (reinterpret_cast<setFloat>(this->setter)(value))
            this->changed = true;
    }

    void set(const char* value)
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_STRING) return;
        if (this->location != NULL)
        {
            char* current = *(reinterpret_cast<char**>(this->location));
            if (strcmp(current, value) != 0)
            {
                strncpy(current, value, STRING_ATTRIBUTE_MAX_LENGTH);
                current[STRING_ATTRIBUTE_MAX_LENGTH] = '\0';
                this->changed = true;
            }
        }
        else
        if (reinterpret_cast<setString>(this->setter)(value))
            this->changed = true;
    }

    void set_sync_to(ClientID client_id)
    {
        GS_ASSERT(this->sync_to == NULL_CLIENT);
        this->sync_to = client_id;
    }

    /* Registration API */

    void add_setter(setInt setter)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_INT);
        this->value_type = ATTRIBUTE_VALUE_INT;
        this->_add_setter(reinterpret_cast<voidFunction>(setter));
    }

    void add_setter(setFloat setter)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_FLOAT);
        this->value_type = ATTRIBUTE_VALUE_FLOAT;
        this->_add_setter(reinterpret_cast<voidFunction>(setter));
    }

    void add_setter(setString setter)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_STRING);
        this->value_type = ATTRIBUTE_VALUE_STRING;
        this->_add_setter(reinterpret_cast<voidFunction>(setter));
    }

    void add_getter(setInt getter)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_INT);
        this->value_type = ATTRIBUTE_VALUE_INT;
        this->_add_getter(reinterpret_cast<voidFunction>(getter));
    }

    void add_getter(setFloat getter)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_FLOAT);
        this->value_type = ATTRIBUTE_VALUE_FLOAT;
        this->_add_getter(reinterpret_cast<voidFunction>(getter));
    }

    void add_getter(setString getter)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_STRING);
        this->value_type = ATTRIBUTE_VALUE_STRING;
        this->_add_getter(reinterpret_cast<voidFunction>(getter));
    }

    void set_location(int* location)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_INT);
        this->value_type = ATTRIBUTE_VALUE_INT;
        this->_set_location(location);
    }

    void set_location(float* location)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_FLOAT);
        this->value_type = ATTRIBUTE_VALUE_FLOAT;
        this->_set_location(location);
    }

    void set_location(char** location)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_STRING);
        this->value_type = ATTRIBUTE_VALUE_STRING;
        this->_set_location(location);
    }

    void set_sync_type(AttributeSyncType sync_type)
    {
        GS_ASSERT(this->sync_type == NULL_ATTRIBUTE_SYNC_TYPE);
        this->sync_type = sync_type;
    }

    /* Networking */

    void send()
    {
        if (this->sync_type == ATTRIBUTE_SYNC_TYPE_PRIVATE) return;

        #define ROUTE_MESSAGE(MSG_TYPE) \
        { do { \
            MSG_TYPE msg; \
            this->_pack_message(&msg); \
            if (this->sync_type == ATTRIBUTE_SYNC_TYPE_ALL) \
                msg.broadcast(); \
            else \
            if (this->sync_type == ATTRIBUTE_SYNC_TYPE_PLAYER) \
            { \
                GS_ASSERT(this->sync_to != NULL_CLIENT); \
                msg.sendToClient(this->sync_to); \
            } \
        } while (0); }

        if (this->value_type == ATTRIBUTE_VALUE_INT)
        {
            ROUTE_MESSAGE(set_attribute_int_StoC);
        }
        else
        if (this->value_type == ATTRIBUTE_VALUE_FLOAT)
        {
            ROUTE_MESSAGE(set_attribute_float_StoC);
        }
        else
        if (this->value_type == ATTRIBUTE_VALUE_STRING)
        {
            ROUTE_MESSAGE(set_attribute_string_StoC);
        }

        #undef ROUTE_MESSAGE

        GS_ASSERT(false);
    }

    void send_changes()
    {
        if (!this->changed) return;
        this->send();
        this->changed = false;
    }

    /* Verification */

    void verify() const
    {
        GS_ASSERT(this->type != NULL_ATTRIBUTE);
        GS_ASSERT(this->group != NULL_ATTRIBUTE_GROUP);
        GS_ASSERT(this->value_type != NULL_ATTRIBUTE_VALUE_TYPE);
        GS_ASSERT(this->name[0] != '\0');
        GS_ASSERT(this->sync_type != NULL_ATTRIBUTE_SYNC_TYPE);
        GS_ASSERT(this->sync_type != ATTRIBUTE_SYNC_TYPE_PLAYER ||
                  this->sync_to != NULL_CLIENT);

        GS_ASSERT((this->location == NULL && (this->getter != NULL **
                                               this->setter != NULL)) ||
                  (this->location != NULL && (this->getter == NULL &&
                                               this->setter == NULL)));
    }

    void verify_other(const Attribute* other) const
    {
        GS_ASSERT(this->getter != other->getter);
        GS_ASSERT(this->setter != other->setter);
        GS_ASSERT(strcmp(this->name, other->name) != 0);
    }

    /* Init */

    Attribute() :
        Property<AttributeType>(NULL_ATTRIBUTE),
        group(NULL_ATTRIBUTE_GROUP), value_type(NULL_ATTRIBUTE_VALUE_TYPE),
        sync_type(NULL_ATTRIBUTE_SYNC_TYPE),
        location(NULL), getter(NULL), setter(NULL),
        changed(false), sync_to(NULL_CLIENT)
    {
    }

    private:

    void _add_setter(voidFunction setter)
    {
        GS_ASSERT(!this->loaded);
        GS_ASSERT(this->setter != NULL);
        this->setter = setter;
    }

    void _add_getter(voidFunction getter)
    {
        GS_ASSERT(!this->loaded);
        GS_ASSERT(this->getter != NULL);
        this->getter = getter;
    }

    void _set_location(void* location)
    {
        GS_ASSERT(!this->loaded);
        GS_ASSERT(this->location == NULL);
        this->location = location;
    }

    void _pack_message(set_attribute_int_StoC* msg)
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_INT);
        msg->attribute_type = this->type;
        msg->value = this->get_int();
    }

    void _pack_message(set_attribute_float_StoC* msg)
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_FLOAT);
        msg->attribute_type = this->type;
        msg->value = this->get_float();
    }

    void _pack_message(set_attribute_string_StoC* msg)
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_STRING);
        msg->attribute_type = this->type;
        strncpy(msg->value, this->get_string(), STRING_ATTRIBUTE_MAX_LENGTH);
        msg->value[STRING_ATTRIBUTE_MAX_LENGTH] = '\0';
    }
};


class Attributes: public Properties<Attribute, AttributeType>
{
    public:

    void verify()
    {
        for (size_t i=0; i<this->max; i++)
            if (this->properties[i].loaded)
                this->properties[i].verify();

        for (size_t i=0; i<this->max-1; i++)
        for (size_t j=i+1; j<this->max; j++)
            if (this->properties[i].loaded && this->properties[j].loaded)
                this->properties[i].verify_other(&this->properties[j]);
    }

    void send()
    {
        for (size_t i=0; i<this->index; i++)
            if (this->properties[i].loaded)
                this->properties[i].send();
    }

    void send_changes()
    {
        for (size_t i=0; i<this->index; i++)
            if (this->properties[i].loaded)
                this->properties[i].send_changes();
    }

    Attributes() :
        Properties<Attribute, AttributeType>(MAX_ATTRIBUTES)
    {}
};


static Attributes* attributes;

/* Boilerplate */

void init()
{
    GS_ASSERT(attributes == NULL);
    attributes = new Attributes;
}

void teardown()
{
    if (attributes != NULL) delete attributes;
}

void verify()
{
    attributes->verify();
}

/* Registration API */

AttributeType def(AttributeGroup group, const char* name)
{
    Attribute* a = attributes->get_next();
    IF_ASSERT(a == NULL) return NULL_ATTRIBUTE;
    a->group = group;
    a->set_name(name);
    return a->type;
}

void set_sync_type(AttributeType type, AttributeSyncType sync_type)
{
    Attribute* a = attributes->_get_any(type);
    IF_ASSERT(a == NULL) return;
    a->set_sync_type(sync_type);
}

void set_location(AttributeType type, int* location)
{
    Attribute* a = attributes->_get_any(type);
    IF_ASSERT(a == NULL) return;
    a->set_location(location);
}

void set_location(AttributeType type, float* location)
{
    Attribute* a = attributes->_get_any(type);
    IF_ASSERT(a == NULL) return;
    a->set_location(location);
}

void set_location(AttributeType type, char** location)
{
    Attribute* a = attributes->_get_any(type);
    IF_ASSERT(a == NULL) return;
    a->set_location(location);
}

void done_loading()
{
    attributes->done_loading();
}

/* Network */

void init_packets()
{
    set_attribute_int_StoC::register_client_packet();
    set_attribute_float_StoC::register_client_packet();
    set_attribute_string_StoC::register_client_packet();
}

#if DC_CLIENT
inline void set_attribute_int_StoC::handle()
{
    Attribute* attr = attributes->get((AttributeType)this->attribute_type);
    IF_ASSERT(attr == NULL) return;
    attr->set((int)this->value);
}

inline void set_attribute_float_StoC::handle()
{
    Attribute* attr = attributes->get((AttributeType)this->attribute_type);
    IF_ASSERT(attr == NULL) return;
    attr->set(this->value);
}

inline void set_attribute_string_StoC::handle()
{
    Attribute* attr = attributes->get((AttributeType)this->attribute_type);
    IF_ASSERT(attr == NULL) return;
    attr->set(this->value);
}
#endif

#if DC_SERVER
inline void set_attribute_int_StoC::handle() {}
inline void set_attribute_float_StoC::handle() {}
inline void set_attribute_string_StoC::handle() {}
#endif

}   // Attributes