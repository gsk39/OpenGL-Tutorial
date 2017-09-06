/**
* @file Entity.cpp
*/
#include "Entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

/**
* �G���e�B�e�B�Ɋւ���R�[�h���i�[���閼�O���.
*/
namespace Entity {
	/**
	* �ړ��E��]�E�g�k�s����擾����.
	*
	* @return TRS�s��.
	*/
	glm::mat4 Entity::CalcModelMatrix() const
	{
		const glm::mat4 t = glm::translate(glm::mat4(), position);
		const glm::mat4 r = glm::mat4_cast(rotation);
		const glm::mat4 s = glm::scale(glm::mat4(), scale);
		return t * r * s;
	}

	/**
	* �������g�������N���X�g����؂藣��.
	*
	* �����͂ǂ��ɂ��ڑ�����Ă��Ȃ���ԂɂȂ�.
	*/
	void Buffer::Link::Remove()
	{
		next->prev = prev;
		prev->next = next;
		prev = this;
		next = this;
	}

	/*�����N�I�u�W�F�N�g�������̎�O�ɒǉ�����.
	*
	* @param p�@�ǉ����郊���N�I�u�W�F�N�g�ւ̃|�C���^.
	*
	* p���������̃����N���X�g����؂藣���A�����̎�O�ɒǉ�����.
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
	* �G���e�B�e�B�o�b�t�@���쐬����.
	*
	* @param maxEntityCount   ������G���e�B�e�B�̍ő吔.
	* @param ubSizePerEntity  �G���e�B�e�B���Ƃ�Uniform Buffer�̃o�C�g��.
	* @param bindingPoint     �G���e�B�e�B�pUBO�̃o�C���f�B���O�|�C���g.
	* @param ubName           �G���e�B�e�B�pUniform Buffer�̖��O.
	*
	* @return �쐬�����G���e�B�e�B�o�b�t�@�ւ̃|�C���^.
	*/
	BufferPtr Buffer::Create(size_t maxEntityCount, GLsizeiptr ubSizePerEntity,
		int bindingPoint, const char* ubName)
	{
		struct Impl : Buffer { Impl() {} ~Impl() {} };
		BufferPtr p = std::make_shared<Impl>();
		if (!p) {
			std::cerr << "WARNING in Entity::Buffer::Create: �o�b�t�@�̍쐬�Ɏ��s." << std::endl;
			return{};
		}
		p->ubo = UniformBuffer::Create(
			maxEntityCount * ubSizePerEntity, bindingPoint, ubName);
		p->buffer.reset(new LinkEntity[maxEntityCount]);
		if (!p->ubo || !p->buffer) {
			std::cerr << "WARNING in Entity::Buffer::Create: �o�b�t�@�̍쐬�Ɏ��s." << std::endl;
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