import pytest

from testsuite.databases import pgsql


# Start the tests via `make test-debug` or `make test-release`


async def test_first_time_users(service_client):
    response = await service_client.post(
        '/v1/hello',
        params={'name': 'userver'},
    )
    assert response.status == 200
    assert response.text == 'Hello, userver!\n'


async def test_db_updates(service_client):
    response = await service_client.post('/v1/hello', params={'name': 'World'})
    assert response.status == 200
    assert response.text == 'Hello, World!\n'

    response = await service_client.post('/v1/hello', params={'name': 'World'})
    assert response.status == 200
    assert response.text == 'Hi again, World!\n'

    response = await service_client.post('/v1/hello', params={'name': 'World'})
    assert response.status == 200
    assert response.text == 'Hi again, World!\n'


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_db_initial_data(service_client):
    response = await service_client.post(
        '/v1/hello',
        params={'name': 'user-from-initial_data.sql'},
    )
    assert response.status == 200
    assert response.text == 'Hi again, user-from-initial_data.sql!\n'


async def test_create_url_success(service_client):
    # First call should create record, second call should do nothing
    for _ in range(2):
        response = await service_client.post('/v1/create', json={
            'url': 'https://dostavka.yandex.ru/',
        })
        assert response.status == 200
        assert response.json() == {
            'short_url': 'http://clck.ru/Xp3k/1',
            'hash': 'Xp3k',
            'version': 1,
        }


async def test_create_url_with_collision(service_client):
    for i in range(1,2):
        response = await service_client.post('/v1/create', json={'url': f'collision{i}'})
        assert response.status == 200
        assert response.json() == {
            'short_url': f'http://clck.ru/AAAA/{i}',
            'hash': 'AAAA',
            'version': i,
        }
