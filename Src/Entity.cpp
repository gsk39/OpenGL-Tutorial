/**
* @file Entity.cpp
*/
#include "Entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

/**
* エンティティに関するコードを格納する名前空間.
*/
namespace Entity {
	/**
	* 移動・回転・拡縮行列を取得する.
	*
	* @return TRS行列.
	*/
	glm::mat4 Entity::CalcModelMatrix() const
	{
		const glm::mat4 t = glm::translate(glm::mat4(), position);
		const glm::mat4 r = glm::mat4_cast(rotation);
		const glm::mat4 s = glm::scale(glm::mat4(), scale);
		return t * r * s;
	}

	/**
	* 自分自身をリンクリストから切り離す.
	*
	* 自分はどこにも接続されていない状態になる.
	*/
	void Buffer::Link::Remove()
	{
		next->prev = prev;
		prev->next = next;
		prev = this;
		next = this;
	}

	/*リンクオブジェクトを自分の手前に追加する.
	*
	* @param p　追加するリンクオブジェクトへのポインタ.
	*
	* pを所属元のリンクリストから切り離し、自分の手前に追加する.
	*/
	void Buffer::Link::Insert(Link* p)
	{
		p->Remove();
		p->prev = prev;
		p->next = this;
		prev->next = p;
		prev = p;
	}

	/**
	* エンティティバッファを作成する.
	*
	* @param maxEntityCount   扱えるエンティティの最大数.
	* @param ubSizePerEntity  エンティティごとのUniform Bufferのバイト数.
	* @param bindingPoint     エンティティ用UBOのバインディングポイント.
	* @param ubName           エンティティ用Uniform Bufferの名前.
	*
	* @return 作成したエンティティバッファへのポインタ.
	*/
	BufferPtr Buffer::Create(size_t maxEntityCount, GLsizeiptr ubSizePerEntity,
		int bindingPoint, const char* ubName)
	{
		struct Impl : Buffer { Impl() {} ~Impl() {} };
		BufferPtr p = std::make_shared<Impl>();
		if (!p) {
			std::cerr << "WARNING in Entity::Buffer::Create: バッファの作成に失敗." << std::endl;
			return{};
		}
		p->ubo = UniformBuffer::Create(
			maxEntityCount * ubSizePerEntity, bindingPoint, ubName);
		p->buffer.reset(new LinkEntity[maxEntityCount]);
		if (!p->ubo || !p->buffer) {
			std::cerr << "WARNING in Entity::Buffer::Create: バッファの作成に失敗." << std::endl;
			return{};
		}
		p->bufferSize = maxEntityCount;
		p->ubSizePerEntity = ubSizePerEntity;
		GLintptr offset = 0;
		const LinkEntity* const end = &p->buffer[maxEntityCount];
		for (LinkEntity* itr = &p->buffer[0]; itr != end; ++itr) {
			itr->uboOffset = offset;
			p->freeList.Insert(itr);
			offset += ubSizePerEntity;
		}
		return p;
	}

	
}