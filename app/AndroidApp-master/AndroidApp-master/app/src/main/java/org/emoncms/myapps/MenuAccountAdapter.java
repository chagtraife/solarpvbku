package org.emoncms.myapps;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 * Handles the Items in the Account List in the navigaton drawer
 */
public class MenuAccountAdapter extends RecyclerView.Adapter<MenuAccountAdapter.ViewHolder> implements AccountListChangeListener {

    private Context mContext;

    private List<MenuOption> menuOptionList;
    private OnNavigationClick onNavigationClick;
    private int currentItemRestoreIndex = -1;


    public class ViewHolder extends RecyclerView.ViewHolder {

        private TextView textView;
        private ImageView imageView;
        private View divider;

        public ViewHolder(View itemView) {
            super(itemView);
            textView = (TextView) itemView.findViewById(R.id.rowText);
            imageView = (ImageView) itemView.findViewById(R.id.rowIcon);
            //imageView.setVisibility(View.GONE);
            divider = itemView.findViewById(R.id.rowDivider);
            divider.setVisibility(View.INVISIBLE);
        }

        public void bind(final MenuOption option, final OnNavigationClick onNavigationClick) {
            Log.d("emon-menu", "calling on bind for " + option.id + " " + option.text);

            if (option != null) {
                textView.setText(option.text);
                imageView.setImageResource(option.icon);
                itemView.setSelected(false);

                if (menuOptionList.indexOf(option) == currentItemRestoreIndex) {
                    textView.setVisibility(View.GONE);
                } else {
                    textView.setVisibility(View.VISIBLE);
                }

                View.OnClickListener onClickListener = new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        Log.d("emon-menu", "clicked " + option.id + " " + textView.getText());
                        onNavigationClick.onClick(option.id);
                    }
                };

                itemView.setOnClickListener(onClickListener);
            }
        }
    }

    /**
     * Context is required
     *
     * @param context
     */
    public MenuAccountAdapter(Context context, OnNavigationClick onNavigationClick) {
        mContext = context;
        this.onNavigationClick = onNavigationClick;
        menuOptionList = new ArrayList<>();
        Log.d("emon-menu", "creating account menu");
        for (Map.Entry<String, String> account : EmonApplication.get().getAccounts().entrySet()) {
            menuOptionList.add(new MenuOption(account.getKey(), R.mipmap.ic_account, account.getValue()));
        }
    }

    @Override
    public void onAddAccount(String id, String name) {
        int newPosition = menuOptionList.size();
        Log.d("emon-menu", "Adding account at position " + newPosition + " with name " + name);
        menuOptionList.add(newPosition,new MenuOption(id, R.mipmap.ic_account, name));


        notifyItemInserted(newPosition);
        notifyDataSetChanged();
    }

    @Override
    public void onDeleteAccount(String id) {
        int position = getPosition(id);
        if (position > -1) {
            menuOptionList.remove(position);
            notifyItemRemoved(position);
        }
    }

    @Override
    public void onUpdateAccount(String id, String name) {
        Log.d("emon-menu", "Updating account at with name " + name);
        int position = getPosition(id);
        if (position > -1) {
            Log.d("menu","Fixing account name" + name);
            menuOptionList.get(position).text = name;
            notifyItemChanged(position);
            //notifyDataSetChanged();
        }
    }

    private int getPosition(String id) {
        int position = -1;
        for (int i = 0; i < menuOptionList.size(); i++) {
            if (menuOptionList.get(i).id.equals(id)) {
                position = i;
                break;
            }
        }
        return position;
    }

    @Override
    public MenuAccountAdapter.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View v = LayoutInflater.from(parent.getContext()).inflate(R.layout.menu_item_page, parent, false);
        return new ViewHolder(v);
    }

    @Override
    public void onBindViewHolder(MenuAccountAdapter.ViewHolder holder, int position) {
        holder.bind(menuOptionList.get(position), onNavigationClick);
    }

    @Override
    public int getItemCount() {
        return menuOptionList.size();
    }

    public static class MenuOption {
        String id;
        int icon;
        String text;

        public MenuOption(String id, int icon, String text) {
            this.id = id;
            this.icon = icon;
            this.text = text;
        }


    }

}