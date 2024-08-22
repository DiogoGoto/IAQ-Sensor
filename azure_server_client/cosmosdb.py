from azure.cosmos.aio import CosmosClient as cosmos_client
from azure.cosmos import PartitionKey, exceptions
import asyncio

endpoint = "https://<name>.azure.com:<xxx>/"
key = "<your_key_here>"
database_name = "<your_database_name>"
container_name = "<your_container_name>"
cosmos_database=[]
cosmos_container=[]


async def get_or_create_db(client, database_name):
    try:
        database_obj  = client.get_database_client(database_name)
        await database_obj.read()
        return database_obj
    except exceptions.CosmosResourceNotFoundError:
        print("Creating database")
        return await client.create_database(database_name)
    
async def get_or_create_container(database_obj, container_name):
    try:        
        todo_items_container = database_obj.get_container_client(container_name)
        await todo_items_container.read()   
        return todo_items_container
    except exceptions.CosmosResourceNotFoundError:
        print("Creating container with lastName as partition key")
        return await database_obj.create_container(
            id=container_name,
            partition_key=PartitionKey(path="/lastName"),
            offer_throughput=400)
    except exceptions.CosmosHttpResponseError:
        raise    

async def populate_container_items(container_obj, items_to_create):
    family_items_to_create = items_to_create
    for family_item in family_items_to_create:
        inserted_item = await container_obj.create_item(body=family_item)
        print("Inserted item for %s family. Item Id: %s" %(inserted_item['lastName'], inserted_item['id']))

async def read_items(container_obj, items_to_read):
    for family in items_to_read:
        item_response = await container_obj.read_item(item=family['id'], partition_key=family['lastName'])
        request_charge = container_obj.client_connection.last_response_headers['x-ms-request-charge']
        print('Read item with id {0}. Operation consumed {1} request units'.format(item_response['id'], (request_charge)))

async def query_items(container_obj, query_text):
    query_items_response = container_obj.query_items(query=query_text)#,
    request_charge = container_obj.client_connection.last_response_headers['x-ms-request-charge']
    items = [item async for item in query_items_response]
    
    return items

    print('Query returned {0} items. Operation consumed {1} request units'.format(len(items), request_charge))
    for item in items:
      print(item,end='\n')

async def run_sample():
    async with cosmos_client(endpoint, credential = key) as client:
        try:
            print("run_sample()->get_or_create_db()")
            database_obj = await get_or_create_db(client, database_name)

            print("run_sample()->get_or_create_container()")
            container_obj = await get_or_create_container(database_obj, container_name)

            #query = 'SELECT * FROM c WHERE c.Body.datetime >= "2024-07-02T15:44:00"'
            query = 'SELECT top 1 * FROM c order by c.Body.datetime DESC'
            print("run_sample()->query_items()")
            items = await query_items(container_obj, query)

            with open('test2.txt','w') as tf:
                for item in items:
                    tf.write(f"{str(item)}\n")
                    #tf.writelines(str(item))

        except exceptions.CosmosHttpResponseError as e:
            print('\nrun_sample has caught an error. {0}'.format(e.message))
        finally:
            print("\nQuickstart complete") 

async def cosmos_query(query):
    async with cosmos_client(endpoint, credential = key) as client:
        items=[]
        global cosmos_database
        global cosmos_container
        try:
            #print("run_sample()->get_or_create_db()")
            #if cosmos_database==[]:
            cosmos_database = await get_or_create_db(client, database_name)

            #print("run_sample()->get_or_create_container()")
            #if cosmos_container==[]:
            cosmos_container = await get_or_create_container(cosmos_database, container_name)

            #query = 'SELECT * FROM c WHERE c.Body.datetime >= "2024-07-02T15:44:00"'
            #query = 'SELECT top 1 * FROM c order by c.Body.datetime DESC'
            print("cosmos_query()->query_items()")
            items = await query_items(cosmos_container, query)

            #fcallback(items)

            # with open('test2.txt','w') as tf:
            #     for item in items:
            #         tf.write(f"{str(item)}\n")
                    #tf.writelines(str(item))

        except exceptions.CosmosHttpResponseError as e:
            print('\nrun_sample has caught an error. {0}'.format(e.message))
        finally:
            print("\nQuickstart complete") 

        return items

if __name__=="__main__":
    loop = asyncio.get_event_loop()
    loop.run_until_complete(run_sample())


