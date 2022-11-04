#include "pch.h"
#include "Player.h"
#include "Inventory.h"
#include "Equipment.h"
#include "DBJobManager.h"
#include "Item.h"
#include "ServerPacketManager.h"

Player::Player()
{
	SetObjectType(Protocol::ObjectType::PLAYER);

	Protocol::Vector vec;
	vec.set_x(0);
	vec.set_y(0);

	SetVector(vec);
	_Inventory = Gnew<Inventory>();
	_Equipment = Gnew<Equipment>();
}

Player::~Player()
{
	SetSession(nullptr);
	Gdelete(_Inventory);
	Gdelete(_Equipment);
}

/*---------------------------------------------------------------------------------------------
이름     : Player::LeaveGame
용도     : 플레이어가 방에서 퇴장했을 때 사용하는 함수
수정자   : 이민규
수정날짜 : 2022.09.19
----------------------------------------------------------------------------------------------*/
void Player::LeaveGame()
{
	DBJobManager::GetInstance()->SavePlayerStatus(this, _Room);
}

/*---------------------------------------------------------------------------------------------
이름     : Player::ItemEquip
용도     : 플레이어의 아이템을 장착시키는 함수
수정자   : 이민규
수정날짜 : 2022.10.28
----------------------------------------------------------------------------------------------*/
void Player::ItemEquip(Protocol::CLIENT_EQUIPITEM* pkt)
{
	Item * item = nullptr;

	if(pkt->equipped() == 1)
		item = GetInventory()->Find(pkt->itemdbid());
	else
		item = GetEquipment()->Find(pkt->itemdbid());

	if (item == nullptr)
		return;

	if (item->GetItemType() == Protocol::ItemType::ITEM_TYPE_CONSUMABLE)
		return;

	// 착용 요청이라면 , 겹치는 부위 해제
	if (pkt->equipped())
	{
		Item* unequipitem = nullptr;

		// 무기 아이템 겹치는 경우
		if (item->GetItemType() == Protocol::ITEM_TYPE_WEAPON)
		{
			unequipitem = GetEquipment()->FindCondition([](Item* item)
				{
					if (item->GetItemType() == Protocol::ITEM_TYPE_WEAPON && item->GetEquip() == 1)
						return true;
					else
						return false;
				});
		}
		// 방어구 아이템 겹치는 경우
		else if (item->GetItemType() == Protocol::ITEM_TYPE_ARMOR)
		{
			Protocol::ArmorType armortype = static_cast<Armor*>(item)->GetArmorType();
			unequipitem = GetEquipment()->FindCondition([&armortype](Item* item)
				{
					if (item->GetItemType() == Protocol::ITEM_TYPE_ARMOR && item->GetEquip() == 1 && static_cast<Armor*>(item)->GetArmorType() == armortype)
						return true;
					else
						return false;
				});
		}

		// 겹치는 아이템 장착해제
		if (unequipitem != nullptr)
		{
			// 메모리 선 적용
			unequipitem->SetEquip(0);
			_Equipment->Erase(unequipitem);

			// 슬롯번호 재 할당 후 다시 인벤토리에 추가
			unequipitem->SetSlot( GetInventory()->GetEmptySlot());
			_Inventory->Add(unequipitem);

			//  DB에 Noti만
			DBJobManager::GetInstance()->EquipItemNoti(this, unequipitem);

			// 클라 통보
			Protocol::SERVER_EQUIPITEM equippkt;
			equippkt.set_itemdbid(unequipitem->GetItemDbID());
			equippkt.set_equipped(unequipitem->GetEquip());
			equippkt.set_slot(unequipitem->GetSlot());
			GetSession()->Send(ServerPacketManager::MakeSendBuffer(equippkt));
		}

		// 메모리 선 적용
		// 착용할 아이템 인벤토리에서 삭제 후 장비창으로 옮김
		_Inventory->Erase(item);
		item->SetEquip(pkt->equipped());

		switch (item->GetItemType())
		{
			case Protocol::ITEM_TYPE_WEAPON:
			{
				item->SetSlot(100);
				break;
			}
			case Protocol::ITEM_TYPE_ARMOR:
			{
				Armor* armor = static_cast<Armor*>(item);
				switch (armor->GetArmorType())
				{
					case Protocol::ARMOR_TYPE_HELMET:
					{
						armor->SetSlot(101);
						break;
					}
					case Protocol::ARMOR_TYPE_ARMOR:
					{
						armor->SetSlot(102);
						break;
					}
					case Protocol::ARMOR_TYPE_BOOTS:
					{
						armor->SetSlot(103);
						break;
					}
				}
			}
		}

		_Equipment->Add(item);

		// DB에 Noti만
		DBJobManager::GetInstance()->EquipItemNoti(this, item);

		// 클라 통보
		Protocol::SERVER_EQUIPITEM equippkt;
		equippkt.set_itemdbid(pkt->itemdbid());
		equippkt.set_equipped(pkt->equipped());

		GetSession()->Send(ServerPacketManager::MakeSendBuffer(equippkt));

		// 스텟 갱신
		RefreshStat();
	}
	// 장착을 해제 할 경우
	else
	{
		// 장비창에서 삭제 후 인벤토리로 전환
		item->SetEquip(0);
		_Equipment->Erase(item);

		// 슬롯번호 재 할당 후 다시 인벤토리에 추가
		item->SetSlot(GetInventory()->GetEmptySlot());
		_Inventory->Add(item);

		//  DB에 Noti만
		DBJobManager::GetInstance()->EquipItemNoti(this, item);

		// 클라 통보
		Protocol::SERVER_EQUIPITEM equippkt;
		equippkt.set_itemdbid(item->GetItemDbID());
		equippkt.set_equipped(item->GetEquip());
		equippkt.set_slot(item->GetSlot());
		GetSession()->Send(ServerPacketManager::MakeSendBuffer(equippkt));

		// 스텟 갱신
		RefreshStat();
	}

}

/*---------------------------------------------------------------------------------------------
이름     : Player::RefreshStat
용도     : 플레이어의 장착 아이템을 가지고 스텟을 설정하는 함수
수정자   : 이민규
수정날짜 : 2022.10.27
----------------------------------------------------------------------------------------------*/
void Player::RefreshStat()
{
	_WeaponAttack = 0;
	_ArmorDefence = 0;

	for(const auto & [id, item] : GetInventory()->GetItems())
	{
		if(item->GetEquip() == 0)
			continue;

		switch (item->GetItemType())
		{
			case Protocol::ITEM_TYPE_WEAPON:
			{
				_WeaponAttack += static_cast<Weapon*>(item)->GetDamage();
				break;
			}
			case Protocol::ITEM_TYPE_ARMOR:
			{
				_ArmorDefence += static_cast<Armor*>(item)->GetDefence();
				break;
			}
		}
	}
}
