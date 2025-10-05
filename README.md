# LegoConnectionTool_Plugin_UE5
The Connection Tool for ALegoActor Inside Unreal Engine 5

- I packaged the plugin and and it is ready to use in other Unreal Engine projects.
- *** How to Use: From the Place Actor panel, search for Lego Actor and drag & drop it into the level. ***
-------------------------------------

--> Features that I developed:
- Customized details panel for ALegoActor to have a separate "Lego Connection Tool" section.
- Customized Shape property; replaced with 4-button selection (with icons) using Slate and IDetailCustomization class.
- Customized Size property; A slider instead of numeric input using Slate and IDetailCustomization class.
- Customized AddConnection, RemoveConnection, Save and Load buttons and drop-downs in the "Lego Connection Tool" section.
- Serializing and Deserializing the level with ALegoActors (And all their connection data).
-------------------------------------

--> CURRENT BUGS & MISSING FEATURE:
- MISSING -> Automation tests for Serialize/Deserialize. My time ran out :(
- MISSING -> Functions for Change Size & Change Color. My time ran out :(

- BUG -> There is a bug when deleting a connected actor, The reference becomes null, but the other connection data remain instead of being cleared.
- BUG -> When loading, if Lego actors with connections are deleted, after loading they respawn but all their connections point to the first Lego actor!
- BUG -> When adding a LegoActor using a Blueprint (from Content Browser), then duplicating it in the level, the GUID is not regenerated, so multiple actors share the same ID; And then the whole save and load won't work properly! This problem won't happen if you place the actor in the level from PlaceActor panel in the editor. Weird!
------------------------------------
