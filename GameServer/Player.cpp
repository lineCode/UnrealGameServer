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
�̸�     : Player::LeaveGame
�뵵     : �÷��̾ �濡�� �������� �� ����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.09.19
----------------------------------------------------------------------------------------------*/
void Player::LeaveGame()
{
	DBJobManager::GetInstance()->SavePlayerStatus(this, _Room);
}

/*---------------------------------------------------------------------------------------------
�̸�     : Player::ItemEquip
�뵵     : �÷��̾��� �������� ������Ű�� �Լ�
������   : �̹α�
������¥ : 2022.10.28
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

	// ���� ��û�̶�� , ��ġ�� ���� ����
	if (pkt->equipped())
	{
		Item* unequipitem = nullptr;

		// ���� ������ ��ġ�� ���
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
		// �� ������ ��ġ�� ���
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

		// ��ġ�� ������ ��������
		if (unequipitem != nullptr)
		{
			// �޸� �� ����
			unequipitem->SetEquip(0);
			_Equipment->Erase(unequipitem);

			// ���Թ�ȣ �� �Ҵ� �� �ٽ� �κ��丮�� �߰�
			unequipitem->SetSlot( GetInventory()->GetEmptySlot());
			_Inventory->Add(unequipitem);

			//  DB�� Noti��
			DBJobManager::GetInstance()->EquipItemNoti(this, unequipitem);

			// Ŭ�� �뺸
			Protocol::SERVER_EQUIPITEM equippkt;
			equippkt.set_itemdbid(unequipitem->GetItemDbID());
			equippkt.set_equipped(unequipitem->GetEquip());
			equippkt.set_slot(unequipitem->GetSlot());
			GetSession()->Send(ServerPacketManager::MakeSendBuffer(equippkt));
		}

		// �޸� �� ����
		// ������ ������ �κ��丮���� ���� �� ���â���� �ű�
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

		// DB�� Noti��
		DBJobManager::GetInstance()->EquipItemNoti(this, item);

		// Ŭ�� �뺸
		Protocol::SERVER_EQUIPITEM equippkt;
		equippkt.set_itemdbid(pkt->itemdbid());
		equippkt.set_equipped(pkt->equipped());

		GetSession()->Send(ServerPacketManager::MakeSendBuffer(equippkt));

		// ���� ����
		RefreshStat();
	}
	// ������ ���� �� ���
	else
	{
		// ���â���� ���� �� �κ��丮�� ��ȯ
		item->SetEquip(0);
		_Equipment->Erase(item);

		// ���Թ�ȣ �� �Ҵ� �� �ٽ� �κ��丮�� �߰�
		item->SetSlot(GetInventory()->GetEmptySlot());
		_Inventory->Add(item);

		//  DB�� Noti��
		DBJobManager::GetInstance()->EquipItemNoti(this, item);

		// Ŭ�� �뺸
		Protocol::SERVER_EQUIPITEM equippkt;
		equippkt.set_itemdbid(item->GetItemDbID());
		equippkt.set_equipped(item->GetEquip());
		equippkt.set_slot(item->GetSlot());
		GetSession()->Send(ServerPacketManager::MakeSendBuffer(equippkt));

		// ���� ����
		RefreshStat();
	}

}

/*---------------------------------------------------------------------------------------------
�̸�     : Player::RefreshStat
�뵵     : �÷��̾��� ���� �������� ������ ������ �����ϴ� �Լ�
������   : �̹α�
������¥ : 2022.10.27
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
