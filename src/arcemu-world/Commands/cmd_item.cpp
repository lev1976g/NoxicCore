/*
 * NoxicCore MMORPG Server
 * Copyright (c) 2011-2014 Crimoxic Team
 * Copyright (c) 2008-2013 ArcEmu Team <http://www.arcemu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "../StdAfx.h"
#include "../Chat.h"
#include "../ObjectMgr.h"
#include "../Master.h"

bool ChatHandler::HandleAddInvItemCommand(const char* args, WorldSession* m_session)
{
	uint32 itemid, count = 1;
	int32 randomprop = 0;
	int32 numadded = 0;

	if(strlen(args) < 1)
	{
		return false;
	}

	if(sscanf(args, "%u %u %d", &itemid, &count, &randomprop) < 1)
	{
		// check for item link
		uint16 ofs = GetItemIDFromLink(args, &itemid);
		if(itemid == 0)
			return false;
		sscanf(args + ofs, "%u %d", &count, &randomprop); // these may be empty
	}

	Player* chr = getSelectedChar(m_session, false);
	if(chr == NULL)
		chr = m_session->GetPlayer();

	ItemPrototype* it = ItemPrototypeStorage.LookupEntry(itemid);
	if(it)
	{
		numadded -= chr->GetItemInterface()->GetItemCount(itemid);
		bool result = false;
		result = chr->GetItemInterface()->AddItemById(itemid, count, randomprop);
		numadded += chr->GetItemInterface()->GetItemCount(itemid);
		if(result == true)
		{
			if(count == 0)
			{
				sGMLog.writefromsession(m_session, "used add item command, item id %u [%s], quantity %u, to %s", it->ItemId, it->Name1, numadded, chr->GetName());
			}
			else
			{
				sGMLog.writefromsession(m_session, "used add item command, item id %u [%s], quantity %u (only %lu added due to full inventory), to %s", it->ItemId, it->Name1, numadded, numadded, chr->GetName());
			}

			char messagetext[512];

			snprintf(messagetext, 512, "Added item %s (id: %d), quantity %u, to %s's inventory.", GetItemLinkByProto(it, m_session->language).c_str(), (unsigned int)it->ItemId, numadded, chr->GetName());
			SystemMessage(m_session, messagetext);
			//snprintf(messagetext, 128, "%s added item %d (%s) to your inventory.", m_session->GetPlayer()->GetName(), (unsigned int)itemid, it->Name1);
			snprintf(messagetext, 512, "%s added item %s, quantity %u, to your inventory.", m_session->GetPlayer()->GetName(), GetItemLinkByProto(it, chr->GetSession()->language).c_str(), numadded);

			SystemMessageToPlr(chr,  messagetext);
		}
		else
		{
			SystemMessageToPlr(chr, "Failed to add item.");
		}
		return true;

	}
	else
	{
		RedSystemMessage(m_session, "Item %d is not a valid item!", itemid);
		return true;
	}
}

bool ChatHandler::HandleRemoveItemCommand(const char* args, WorldSession* m_session)
{
	uint32 item_id;
	int32 count, ocount;
	int argc = sscanf(args, "%u %u", (unsigned int*)&item_id, (unsigned int*)&count);
	if(argc == 1)
		count = 1;
	else if(argc != 2 || !count)
		return false;

	ocount = count;
	Player* plr = getSelectedChar(m_session, true);
	if(!plr) return true;

	// loop until they're all gone.
	int32 loop_count = 0;
	int32 start_count = plr->GetItemInterface()->GetItemCount(item_id, true);
	int32 start_count2 = start_count;
	if(count > start_count)
		count = start_count;

	while(start_count >= count && (count > 0) && loop_count < 20)	 // Prevent a loop here.
	{
		plr->GetItemInterface()->RemoveItemAmt(item_id, count);
		start_count2 = plr->GetItemInterface()->GetItemCount(item_id, true);
		count -= (start_count - start_count2);
		start_count = start_count2;
		++loop_count;
	}

	ItemPrototype* iProto	= ItemPrototypeStorage.LookupEntry(item_id);

	if(iProto)
	{
		sGMLog.writefromsession(m_session, "used remove item %s (id: %u) count %u from %s", iProto->Name1, item_id, ocount, plr->GetName());
		BlueSystemMessage(m_session, "Removing %u copies of item %s (id: %u) from %s's inventory.", ocount, GetItemLinkByProto(iProto, m_session->language).c_str(), item_id, plr->GetName());
		BlueSystemMessage(plr->GetSession(), "%s removed %u copies of item %s from your inventory.", m_session->GetPlayer()->GetName(), ocount, GetItemLinkByProto(iProto, plr->GetSession()->language).c_str());
	}
	else RedSystemMessage(m_session, "Cannot remove non valid item id: %u .", item_id);

	return true;
}

bool ChatHandler::HandleAddItemSetCommand(const char* args, WorldSession* m_session)
{
	uint32 setid = (args ? atoi(args) : 0);
	if(!setid)
	{
		RedSystemMessage(m_session, "You must specify a setid.");
		return true;
	}

	Player* chr = getSelectedChar(m_session);
	if(chr == NULL)
	{
		RedSystemMessage(m_session, "Unable to select character.");
		return true;
	}

	ItemSetEntry* entry = dbcItemSet.LookupEntryForced(setid);
	std::list<ItemPrototype*>* l = objmgr.GetListForItemSet(setid);
	if(!entry || !l)
	{
		RedSystemMessage(m_session, "Invalid item set.");
		return true;
	}
	//const char* setname = sItemSetStore.LookupString(entry->name);
	BlueSystemMessage(m_session, "Searching item set %u...", setid);
	uint32 start = getMSTime();
	sGMLog.writefromsession(m_session, "used add item set command, set %u, target %s", setid, chr->GetName());
	for(std::list<ItemPrototype*>::iterator itr = l->begin(); itr != l->end(); ++itr)
	{
		Item* itm = objmgr.CreateItem((*itr)->ItemId, m_session->GetPlayer());
		if(!itm) continue;
		if(itm->GetProto()->Bonding == ITEM_BIND_ON_PICKUP)
		{
			if(itm->GetProto()->Flags & ITEM_FLAG_ACCOUNTBOUND) // don't "Soulbind" account-bound items
				itm->AccountBind();
			else
				itm->SoulBind();
		}

		if(!chr->GetItemInterface()->AddItemToFreeSlot(itm))
		{
			m_session->SendNotification("No free slots left!");
			itm->DeleteMe();
			return true;
		}
		else
		{
			//SystemMessage(m_session, "Added item: %s [%u]", (*itr)->Name1, (*itr)->ItemId);
			SlotResult* le = chr->GetItemInterface()->LastSearchResult();
			chr->SendItemPushResult(false, true, false, true, le->ContainerSlot, le->Slot, 1 , itm->GetEntry(), itm->GetItemRandomSuffixFactor(), itm->GetItemRandomPropertyId(), itm->GetStackCount());
		}
	}
	GreenSystemMessage(m_session, "Added set to inventory complete. Time: %u ms", getMSTime() - start);
	return true;
}

bool ChatHandler::HandleRepairItemsCommand(const char* args, WorldSession* m_session)
{
	Item* pItem;
	Container* pContainer;
	Player* plr;
	uint32 j, i;

	plr = getSelectedChar(m_session, false);
	if(plr == NULL) return false;

	for(i = 0; i < MAX_INVENTORY_SLOT; i++)
	{
		pItem = plr->GetItemInterface()->GetInventoryItem(static_cast<uint16>(i));
		if(pItem != NULL)
		{
			if(pItem->IsContainer())
			{
				pContainer = TO< Container* >(pItem);
				for(j = 0; j < pContainer->GetProto()->ContainerSlots; ++j)
				{
					pItem = pContainer->GetItem(static_cast<uint16>(j));
					if(pItem != NULL)
						RepairItem2(plr, pItem);
				}
			}
			else
			{
				if(pItem->GetProto()->MaxDurability > 0 && i < INVENTORY_SLOT_BAG_END && pItem->GetDurability() <= 0)
				{
					RepairItem2(plr, pItem);
					plr->ApplyItemMods(pItem, static_cast<uint16>(i), true);
				}
				else
				{
					RepairItem2(plr, pItem);
				}
			}
		}
	}

	SystemMessage(m_session, "Items Repaired");
	return true;
}

bool ChatHandler::HandleShowItems(const char* args, WorldSession* m_session)
{
	string q;
	Player* plr = getSelectedChar(m_session, true);
	if(!plr) return true;
	BlueSystemMessage(m_session, "Listing items for player %s", plr->GetName());
	int itemcount = 0;
	ItemIterator itr(plr->GetItemInterface());
	itr.BeginSearch();
	for(; !itr.End(); itr.Increment())
	{
		if(!(*itr))
			return false;
		itemcount++;
		SendItemLinkToPlayer((*itr)->GetProto(), m_session, true, plr, m_session->language);
	}
	itr.EndSearch();
	BlueSystemMessage(m_session, "Listed %d items for player %s", itemcount, plr->GetName());

	sGMLog.writefromsession(m_session, "used show items command on %s,", plr->GetName());

	return true;
}