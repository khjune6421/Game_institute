#include "ObjectManager.h"

Object::ObjectId ObjectManager::CreateObject()
{
	Object::ObjectId id = nextId++;
	objects.emplace(id, Object(id));
	return id;
}

void ObjectManager::DestroyObject(Object::ObjectId id)
{
	objects.erase(id);
	spriteAnimators.erase(id);
}

Object* ObjectManager::GetObject(Object::ObjectId id)
{
	auto it = objects.find(id);
	return (it != objects.end()) ? &it->second : nullptr;
}

SpriteAnimator* ObjectManager::GetSpriteAnimator(Object::ObjectId id)
{
	auto it = spriteAnimators.find(id);
	return (it != spriteAnimators.end()) ? &it->second : nullptr;
}

void ObjectManager::AddSpriteAnimator(Object::ObjectId id, const SpriteAnimator& animator)
{
	spriteAnimators[id] = animator;
}

void ObjectManager::SetObjectJsonPath(Object::ObjectId id, const std::wstring& path)
{
	objectJsonPaths[id] = path;
}

wstring ObjectManager::GetObjectJsonPath(Object::ObjectId id) const
{
	auto it = objectJsonPaths.find(id);
	return (it != objectJsonPaths.end()) ? it->second : L"";
}

Transform* ObjectManager::GetTransform(Object::ObjectId id)
{
	auto it = transforms.find(id);
	return (it != transforms.end()) ? &it->second : nullptr;
}

void ObjectManager::AddTransform(Object::ObjectId id, const Transform& transform)
{
	transforms[id] = transform;
}